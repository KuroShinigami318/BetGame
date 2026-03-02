#pragma once

class IAnimable;
class ICard;
struct LogicConfig;

namespace logic
{
enum class StagePhase : uint8_t;
}

class IStageLogic
{
public:
	struct RollResult
	{
		virtual ~RollResult() = default;
		size_t winningCardIndex;
	};

protected:
	struct SignalKey;

public:
	virtual ~IStageLogic() = default;
	virtual void UpdateLogicConfig(const LogicConfig&) = 0;
	virtual void AddCardComponent(ICard&) = 0;
	virtual void RegisterAnimableComponent(const IAnimable&) = 0;
	virtual void UnregisterAnimableComponent(const IAnimable&) = 0;
	virtual void BidOnCard(const size_t& i_id, const uint32_t& i_bidValue) = 0;
	virtual std::unique_ptr<RollResult> RollCards(bool i_ignoreHitRate = false) = 0;
	virtual void Reset(bool = false) = 0;
	virtual uint32_t GetDefaultBid() const = 0;

public:
	utils::Signal_public<void(const int&), SignalKey> sig_onBidChanged;
	utils::Signal_public<void(), SignalKey> sig_onAnimationStarted;
	utils::Signal_public<void(), SignalKey> sig_onAnimationFinished;
	utils::Signal_public<void(const logic::StagePhase&), SignalKey> sig_onStagePhaseChanged;
};