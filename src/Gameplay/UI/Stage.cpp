#include "stdafx.h"
#include "Gameplay/UI/Stage.h"
#include "Control/ActionCode.h"
#include "UI/IUIManager.h"
#include "Gameplay/Logic/StagePhase.h"
#include "MacrosUtils.h"
#include "attribute.h"
#include "TimerDelayer.h"
#include "system_clock.h"

namespace
{
constexpr const float LOCAL_ANIMATION_DURATION = 3000.f;
constexpr const float ANIMATION_SPEED = 3.f;
}

Stage::Stage(const UIContext& i_uiContext, IStageLogic& i_stageLogic, const uint16_t& i_width, const uint16_t& i_height)
	: IStage(i_uiContext)
	, m_stageLogic(i_stageLogic)
	, m_width(i_width)
	, m_height(i_height)
	, m_isRollStarted(false)
	, m_isOnAnimation(false)
	, m_isOnLocalAnimation(false)
	, m_currentIndex(0.f)
	, m_animationDelayer(utils::make_unique<utils::TimerDelayer>(LOCAL_ANIMATION_DURATION))
{
	m_animationDelayer->Stop();
	m_connections.push_back(m_uiContext.systemClock.sig_onTick.Connect(&Stage::Update, this));
	m_connections.push_back(m_stageLogic.sig_onAnimationFinished.Connect(&Stage::OnAnimtionFinished, this));
	m_connections.push_back(m_stageLogic.sig_onAnimationStarted.Connect(&Stage::OnAnimtionStarted, this));
	m_connections.push_back(m_stageLogic.sig_onStagePhaseChanged.Connect(&Stage::OnStagePhaseChanged, this));
	m_connections.push_back(m_animationDelayer->sig_onExpired.Connect(&Stage::OnLocalAnimationFinished, this));
}

void Stage::AddCardComponent(utils::unique_ref<IUIComponent> i_cardComponent)
{
	m_cardComponents.push_back(std::move(i_cardComponent));
}

void Stage::SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle)
{
	i_renderStyle->BindAttribute("width", utils::attribute::make_bind_attribute(m_width));
	i_renderStyle->BindAttribute("height", utils::attribute::make_bind_attribute(m_height));
	i_renderStyle->BindAttribute("cardComponents", utils::attribute::make_bind_attribute(m_cardComponents, {nullptr}));
	IUIComponent::SetRenderStyle(std::move(i_renderStyle));
}

void Stage::Render(RendererT& o_renderStream) const
{
	if (m_renderStyle)
	{
		m_renderStyle->Render(o_renderStream);
	}
	o_renderStream << utils::Format("\033[{}B", m_height);
}

bool Stage::ProcessInput(const std::string& i_input) const
{
	static bool toggleIgnoreHitRate = false;
	switch (i_input[0])
	{
	NOT_RELEASE(
	case 'i':
	{
		toggleIgnoreHitRate = !toggleIgnoreHitRate;
		return true;
	})
	default:
	if (i_input[0] > '0' && i_input[0] <= '9')
	{
		uint8_t cardIndex = i_input[0] - '1';
		m_stageLogic.BidOnCard(cardIndex, 1);
		return true;
	}
	else
	{
		std::vector<std::string> inputActionMap = m_uiContext.uiManager.GetInputActionMap(ActionCode::Enter);
		for (const std::string& inputAction : inputActionMap)
		{
			if (i_input == inputAction)
			{
				if (const_cast<Stage*>(this)->m_currentRollResult = m_stageLogic.RollCards(toggleIgnoreHitRate))
				{
					const_cast<Stage*>(this)->m_isRollStarted = true;
					return true;
				}
			}
		}
	}
	break;
	}
	return false;
}

utils::unique_ref<IComponent> Stage::Clone()
{
	return utils::make_unique<Stage>(m_uiContext, m_stageLogic, m_width, m_height);
}

void Stage::Update(float delta)
{
	m_animationDelayer->Update(delta);
	if (!m_currentRollResult)
	{
		return;
	}
	size_t currentCardIndex = size_t(m_currentIndex) % m_cardComponents.size();
	if (!m_isOnLocalAnimation && currentCardIndex == m_currentRollResult->winningCardIndex)
	{
		m_currentRollResult.reset();
		return;
	}
	m_cardComponents[currentCardIndex]->OnFocusLost();
	m_currentIndex += delta * ANIMATION_SPEED;
	m_cardComponents[size_t(m_currentIndex) % m_cardComponents.size()]->OnFocusGained();
}

void Stage::OnAnimtionFinished()
{
	m_isOnAnimation = false;
	m_cardComponents[size_t(m_currentIndex) % m_cardComponents.size()]->OnFocusLost();
	m_currentIndex = 0.f;
	if (m_isRollStarted)
	{
		m_isRollStarted = false;
		m_stageLogic.Reset(true);
	}
}

void Stage::OnAnimtionStarted()
{
	m_isOnAnimation = true;
}

void Stage::OnLocalAnimationFinished()
{
	m_isOnLocalAnimation = false;
}

void Stage::OnStagePhaseChanged(const logic::StagePhase& i_phase)
{
	switch (i_phase)
	{
	case logic::StagePhase::RollStarted:
	{
		m_isOnAnimation = true;
		m_isOnLocalAnimation = true;
		m_animationDelayer->Reset();
	}
	break;
	}
}
