#include "stdafx.h"
#include "Gameplay/Logic/StageLogic.h"
#include "Gameplay/Logic/ICard.h"
#include "Gameplay/Logic/LogicConfig.h"
#include "Gameplay/Logic/StagePhase.h"
#include "Gameplay/UI/IAnimable.h"

namespace
{
template <typename RandomGeneratorT, typename T = decltype(std::declval<RandomGeneratorT>().operator()())> requires std::is_integral_v<T>
T RandomUniqueValue(std::unordered_set<T>& set, RandomGeneratorT& i_randomGenerator, const T& i_min, const T& i_max)
{
	T randomValue;
	do
	{
		randomValue = RandomValue(i_randomGenerator, i_min, i_max);
	} while (set.find(randomValue) != set.end());
	set.insert(randomValue);
	return randomValue;
}

template <typename RandomGeneratorT, typename T = decltype(std::declval<RandomGeneratorT>().operator()())> requires std::is_integral_v<T>
T RandomValue(RandomGeneratorT& i_randomGenerator, const T& i_min, const T& i_max)
{
	return (i_randomGenerator() % (i_max - i_min)) + i_min;
}

struct SignalLogicEmitter : private IStageLogic
{
	static void EmitOnPhaseChanged(IStageLogic& stageLogic, const logic::StagePhase& stagePhase)
	{
		utils::Access<IStageLogic::SignalKey>(stageLogic.sig_onStagePhaseChanged).Emit(stagePhase);
	}
	static void EmitOnBidChanged(IStageLogic& stageLogic, const uint32_t& bid)
	{
		utils::Access<IStageLogic::SignalKey>(stageLogic.sig_onBidChanged).Emit(bid);
	}
};

struct RollFinishedResult : public IStageLogic::RollResult
{
	RollFinishedResult(IStageLogic& i_stageLogic, const size_t& i_winningCardIndex)
		: stageLogic(i_stageLogic)
	{
		winningCardIndex = i_winningCardIndex;
	}
	~RollFinishedResult()
	{
		SignalLogicEmitter::EmitOnPhaseChanged(stageLogic, logic::StagePhase::RollFinished);
	}

private:
	IStageLogic& stageLogic;
};
}

struct AnimableComponentHolder
{
	const IAnimable& animable;
	std::vector<utils::Connection> sig_onAnimationConnections;
	AnimableComponentHolder(const IAnimable& i_animable)
		: animable(i_animable)
	{
	}
};

StageLogic::StageLogic(utils::unique_ref<LogicConfig>&& i_logicConfig)
	: m_finishedAnimationRemaining(0)
	, m_randomGenerator({ {2, 0.5} }, m_randomGeneratorErrorCode)
	, m_logicConfig(std::move(i_logicConfig))
	, m_totalBid(0)
	, m_totalMultiplier(0)
{
}

void StageLogic::UpdateLogicConfig(const LogicConfig& i_logicConfig)
{
	*m_logicConfig = i_logicConfig;
}

void StageLogic::ReserveCardComponentCount(const size_t& i_count)
{
	m_cardComponents.resize(i_count);
}

void StageLogic::SetCardComponent(ICard& i_card, const size_t& i_index)
{
	m_cardDestroyedConnections.push_back(i_card.sig_onComponentDestroyed.Connect(&StageLogic::OnCardDestroyed, this, i_index));
	m_cardComponents[i_index] = &i_card;
}

void StageLogic::RegisterAnimableComponent(const IAnimable& i_animable)
{
	if (m_animableComponents.find(&i_animable) != m_animableComponents.end())
	{
		return;
	}

	m_animableComponents.emplace(&i_animable, utils::make_unique<AnimableComponentHolder>(i_animable));
	utils::unique_ref<AnimableComponentHolder>& animableComponentHolder = m_animableComponents.find(&i_animable)->second;
	animableComponentHolder->sig_onAnimationConnections.push_back(i_animable.sig_onAnimationStarted.Connect(&StageLogic::OnAnimableComponentAnimationStarted, this, i_animable));
	animableComponentHolder->sig_onAnimationConnections.push_back(i_animable.sig_onAnimationFinished.Connect(&StageLogic::OnAnimableComponentAnimationFinished, this, i_animable));
}

void StageLogic::UnregisterAnimableComponent(const IAnimable& i_animable)
{
	auto iter = m_animableComponents.find(&i_animable);
	if (iter == m_animableComponents.end())
	{
		return;
	}
	m_animableComponents.erase(iter);
}

void StageLogic::BidOnCard(const size_t& i_id, const uint32_t& i_bidValue)
{
	if (i_id >= m_cardComponents.size())
	{
		return;
	}
	m_cardComponents[i_id]->Bid(i_bidValue);
	m_totalBid += i_bidValue;
	const int bidDecrement = -1 * i_bidValue;
	utils::Access<SignalKey>(sig_onBidChanged).Emit(bidDecrement);
}

std::unique_ptr<IStageLogic::RollResult> StageLogic::RollCards(bool i_ignoreHitRate)
{
	if (m_totalBid == 0)
	{
		return nullptr;
	}

	int bidSum = 0;
	size_t winningCardIndex = -1;
	float totalHitRate = 0.f;
	std::vector<RandomGeneratorT::weight_type> specialWeights;
	for (size_t index = 0; index < m_cardComponents.size(); ++index)
	{
		ICard* card = m_cardComponents[index];
		if (card == nullptr)
		{
			break;
		}
		const float delta = float(card->GetBid() * card->GetMultiplierValue()) / (m_totalBid * m_totalMultiplier);
		const float hitRate = i_ignoreHitRate ? 1.0f : (m_logicConfig->normBidHitRate * (1.f - delta)) / (m_cardComponents.size() / static_cast<float>(m_logicConfig->difficulty));
		if (delta > 0)
		{
			totalHitRate += hitRate;
			specialWeights.emplace_back((uint16_t)index, hitRate);
		}
		if (totalHitRate > 1.f)
		{
			specialWeights.pop_back();
			break;
		}
	}
	m_rollGenerator.SetSpecialWeights(specialWeights, m_randomGeneratorErrorCode);
	m_rollGenerator.BuildDiscreteDistribution(0, (uint16_t)m_cardComponents.size() - 1, m_randomGeneratorErrorCode);
	ASSERT(m_randomGeneratorErrorCode == RandomGeneratorT::ErrorCode::None);
	winningCardIndex = m_rollGenerator();
	bidSum = m_cardComponents[winningCardIndex]->GetBid() * m_cardComponents[winningCardIndex]->GetMultiplierValue();
	utils::Access<SignalKey>(sig_onStagePhaseChanged).Emit(logic::StagePhase::RollStarted);
	utils::Access<SignalKey>(sig_onBidChanged).Emit(bidSum);
	return std::make_unique<RollFinishedResult>(*this, winningCardIndex);
}

void StageLogic::Reset(bool shouldDecreaseHitRate)
{
	const size_t requiredDistributionSize = m_cardComponents.size() * 2;
	if ((m_randomGenerator.max)() < requiredDistributionSize)
	{
		RandomGeneratorT::ErrorCode errorCode;
		m_randomGenerator.BuildDiscreteDistribution(RandomGeneratorT::defaultMinVal, requiredDistributionSize, errorCode);
	}
	//std::unordered_set<uint16_t> uniqueRolls;
	m_totalBid = 0;
	m_totalMultiplier = 0;
	for (ICard* card : m_cardComponents)
	{
		card->ResetBid(0);
		//card->SetMultiplier(RandomUniqueValue(uniqueRolls, m_randomGenerator, (uint16_t)1, (uint16_t)m_cardComponents.size()));
		card->SetMultiplier(m_randomGenerator());
		m_totalMultiplier += card->GetMultiplierValue();
	}

	if (shouldDecreaseHitRate)
	{
		m_logicConfig->normBidHitRate -= m_logicConfig->hitRateStep;
		if (m_logicConfig->normBidHitRate <= 0.f)
		{
			m_logicConfig->normBidHitRate = m_logicConfig->defaultHitRate;
		}
	}
}

uint32_t StageLogic::GetDefaultBid() const
{
	return m_logicConfig->defaultBid;
}

uint16_t StageLogic::GetCardCount() const
{
	return m_logicConfig->cardCount;
}

void StageLogic::OnAnimableComponentAnimationFinished(const IAnimable& i_animable)
{
	if (m_animableComponents.find(&i_animable) == m_animableComponents.end())
	{
		return;
	}
	if (--m_finishedAnimationRemaining <= 0)
	{
		ASSERT_PLAIN_MSG(m_finishedAnimationRemaining == 0, "Unbalanced animation between started and finished. More {} finished animations than started.", std::abs(m_finishedAnimationRemaining));
		m_finishedAnimationRemaining = 0;
		utils::Access<SignalKey>(sig_onAnimationFinished).Emit();
	}
}

void StageLogic::OnAnimableComponentAnimationStarted(const IAnimable& i_animable)
{
	if (m_animableComponents.find(&i_animable) == m_animableComponents.end())
	{
		return;
	}
	if (m_finishedAnimationRemaining++ == 0)
	{
		utils::Access<SignalKey>(sig_onAnimationStarted).Emit();
	}
}

void StageLogic::OnCardDestroyed(size_t i_index)
{
	if (i_index >= m_cardComponents.size())
	{
		return;
	}
	m_cardComponents[i_index] = nullptr;
}