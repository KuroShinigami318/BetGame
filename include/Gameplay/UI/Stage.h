#pragma once

#include "IStage.h"
#include "Control/IInputRelay.h"
#include "Gameplay/Logic/IStageLogic.h"

namespace utils
{
class TimerDelayer;
}

class Stage : public IStage, public IInputRelay
{
public:
	Stage(const UIContext& i_uiContext, IStageLogic& i_stageLogic, const uint16_t& i_width, const uint16_t& i_height);
	void AddCardComponent(utils::unique_ref<IUIComponent>) override;
	void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) override;
	void Render(RendererT& o_renderStream) const override;
	bool ProcessInput(const std::string&) const override;
	utils::unique_ref<IComponent> Clone() override;

private:
	void Update(float);
	void OnAnimtionFinished();
	void OnAnimtionStarted();
	void OnLocalAnimationFinished();
	void OnStagePhaseChanged(const logic::StagePhase&);

private:
	IStageLogic& m_stageLogic;
	uint16_t m_width;
	uint16_t m_height;
	bool m_isRollStarted;
	bool m_isOnAnimation;
	bool m_isOnLocalAnimation;
	std::optional<float> m_currentIndex;
	utils::unique_ref<utils::TimerDelayer> m_animationDelayer;
	std::vector<utils::unique_ref<IUIComponent>> m_cardComponents;
	std::vector<utils::Connection> m_connections;
	std::unique_ptr<IStageLogic::RollResult> m_currentRollResult;
	utils::unique_ref<IRenderStyle> m_winInfoRenderStyle;
};