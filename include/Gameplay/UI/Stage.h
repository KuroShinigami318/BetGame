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
	void ReserveCardComponentCount(const size_t& i_count) override;
	void SetCardComponent(utils::unique_ref<IUIComponent> i_card, const size_t& i_index) override;
	void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) override;
	void Render(RendererT& o_renderStream) const override;
	bool ProcessInput(const std::string&) const override;
	void InitializeInputHints(IInputHints& i_inputHints) const override;
	utils::unique_ref<IComponent> Clone() override;
	void SetWidth(const uint16_t& i_width) override { m_width = i_width; }
    void SetHeight(const uint16_t& i_height) override { m_height = i_height; }
    uint16_t GetWidth() const override { return m_width; }
    uint16_t GetHeight() const override { return m_height; }

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
	std::vector<utils::unique_ptr<IUIComponent>> m_cardComponents;
	std::vector<utils::Connection> m_connections;
	std::unique_ptr<IStageLogic::RollResult> m_currentRollResult;
	utils::unique_ref<IRenderStyle> m_winInfoRenderStyle;
};