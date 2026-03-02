#include "stdafx.h"
#include "Gameplay/UI/BidComponent.h"
#include "attribute.h"
#include "Gameplay/Logic/IStageLogic.h"
#include "Gameplay/Logic/StagePhase.h"
#include "system_clock.h"
#include <cmath>

namespace
{
constexpr const float BID_CHANGE_SPEED = 1.f;
}

BidComponent::BidComponent(const UIContext& i_uiContext, IStageLogic& i_stageLogic, const uint16_t& i_width, const uint16_t& i_height)
	: IBid(i_uiContext)
	, m_stageLogic(i_stageLogic)
	, m_width(i_width)
	, m_height(i_height)
	, m_bid(m_stageLogic.GetDefaultBid())
	, m_accumulatedRatio(0.f)
	, m_currentStagePhase(logic::StagePhase::RollFinished)
{
	m_connections.push_back(m_stageLogic.sig_onBidChanged.Connect(&IBid::SetBid, this));
	m_connections.push_back(m_stageLogic.sig_onStagePhaseChanged.Connect(&BidComponent::OnStagePhaseChanged, this));
	m_connections.push_back(m_uiContext.systemClock.sig_onTick.Connect(&BidComponent::Update, this));
	m_stageLogic.RegisterAnimableComponent(*this);
}

void BidComponent::SetBid(const int& i_bidDelta)
{
	m_optOldBid = float(m_bid);
	m_optNewBid = float(m_bid + i_bidDelta);
}

const int& BidComponent::GetBid() const
{
	return m_bid;
}

void BidComponent::SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle)
{
	struct text{} _text;
	i_renderStyle->BindAttribute("text", utils::attribute::make_value_attribute(_text, { [this](const text&) { return utils::Format("{} coin(s)", m_bid); }}));
	i_renderStyle->BindAttribute("width", utils::attribute::make_bind_attribute(m_width));
	i_renderStyle->BindAttribute("height", utils::attribute::make_bind_attribute(m_height));
	IUIComponent::SetRenderStyle(std::move(i_renderStyle));
}

void BidComponent::Render(RendererT& o_renderStream) const
{
	if (m_renderStyle)
	{
		m_renderStyle->Render(o_renderStream);
	}
	o_renderStream << utils::Format("\033[{}B", m_height);
}

utils::unique_ref<IComponent> BidComponent::Clone()
{
	return utils::make_unique<BidComponent>(m_uiContext, m_stageLogic, m_width, m_height);
}

void BidComponent::OnStagePhaseChanged(const logic::StagePhase& i_newStagePhase)
{
	if (m_currentStagePhase != i_newStagePhase)
	{
		m_currentStagePhase = i_newStagePhase;
	}
}

void BidComponent::Update(float i_delta)
{
	if (m_optNewBid.has_value() && m_currentStagePhase == logic::StagePhase::RollFinished)
	{
		if (m_accumulatedRatio == 0.f)
		{
			utils::Access<IAnimable::SignalKey>(sig_onAnimationStarted).Emit();
		}
		m_accumulatedRatio += std::abs(*m_optNewBid - *m_optOldBid) * BID_CHANGE_SPEED * i_delta;
		if (m_accumulatedRatio >= 1.f || m_optNewBid == m_optOldBid)
		{
			FinishBidAnimation();
		}
		else
		{
			m_bid = std::lerp(*m_optOldBid, *m_optNewBid, m_accumulatedRatio);
		}
	}
}

void BidComponent::FinishBidAnimation()
{
	m_accumulatedRatio = 0.f;
	m_bid = *m_optNewBid;
	m_optOldBid.reset();
	m_optNewBid.reset();
	utils::Access<IAnimable::SignalKey>(sig_onAnimationFinished).Emit();
}
