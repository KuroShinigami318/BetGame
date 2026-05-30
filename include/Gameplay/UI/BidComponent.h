#pragma once

#include "IBid.h"
#include "Control/IInputRelay.h"
#include "Gameplay/UI/IAnimable.h"

class IStageLogic;

namespace logic
{
enum class StagePhase : uint8_t;
}

class BidComponent : public IBid, public IAnimable
{
public:
	~BidComponent();
	BidComponent(const UIContext& i_uiContext, IStageLogic& i_stageLogic, const uint16_t& i_width, const uint16_t& i_height);
	void SetBid(const int&) override;
	const int& GetBid() const override;
	void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) override;
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;
	void SetWidth(const uint16_t& i_width) override { m_width = i_width; }
    void SetHeight(const uint16_t& i_height) override { m_height = i_height; }
    uint16_t GetWidth() const override { return m_width; }
    uint16_t GetHeight() const override { return m_height; }

private:
	void OnStagePhaseChanged(const logic::StagePhase& i_newStagePhase);
	void Update(float);
	void FinishBidAnimation();
	void FinishUpdateBid();

private:
	IStageLogic& m_stageLogic;
	uint16_t m_width;
	uint16_t m_height;
	int m_bid;
	std::optional<float> m_optOldBid;
	std::optional<float> m_optNewBid;
	float m_accumulatedRatio;
	logic::StagePhase m_currentStagePhase;
	std::vector<utils::Connection> m_connections;
};