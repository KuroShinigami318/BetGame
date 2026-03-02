#pragma once

#include "IStageLogic.h"
#include "random_generator.h"

struct AnimableComponentHolder;
struct LogicConfig;

class StageLogic : public IStageLogic
{
private:
	using RandomGeneratorT = utils::RandomGenerator<uint16_t, 1, sizeof(uint16_t), std::mt19937, std::discrete_distribution<uint16_t>>;

public:
	StageLogic(utils::unique_ref<LogicConfig>&&);
	void UpdateLogicConfig(const LogicConfig& i_logicConfig) override;
	void AddCardComponent(ICard& i_card) override;
	void RegisterAnimableComponent(const IAnimable& i_animable) override;
	void UnregisterAnimableComponent(const IAnimable& i_animable) override;
	void BidOnCard(const size_t& i_id, const uint32_t& i_bidValue) override;
	std::unique_ptr<IStageLogic::RollResult> RollCards(bool i_ignoreHitRate = false) override;
	void Reset(bool) override;
	uint32_t GetDefaultBid() const override;

private:
	void OnAnimableComponentAnimationFinished(const IAnimable& i_animable);
	void OnAnimableComponentAnimationStarted(const IAnimable& i_animable);

private:
	std::vector<ICard*> m_cardComponents;
	std::unordered_map<const IAnimable*, utils::unique_ref<AnimableComponentHolder>> m_animableComponents;
	long long m_finishedAnimationRemaining;
	RandomGeneratorT::ErrorCode m_randomGeneratorErrorCode;
	RandomGeneratorT m_randomGenerator;
	utils::unique_ref<LogicConfig> m_logicConfig;
	uint32_t m_totalBid;
	uint32_t m_totalMultiplier;
};