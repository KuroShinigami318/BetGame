#pragma once
#include "UI/WindowBase.h"

class IUIGameplayFactory;
class ILogicGameplayFactory;
class IStageLogic;

enum class UIComponentType : uint8_t;

namespace logic
{
enum class StagePhase : uint8_t;
}

class GameplayWindow : public WindowBase
{
public:
	GameplayWindow(const UIContext& i_uiContext, ILogicGameplayFactory& i_logicFactory, IUIGameplayFactory& i_uiGameplayFactory);
	~GameplayWindow();
	bool ProcessInput(const std::string& input) const override;
	utils::unique_ref<IComponent> Clone() override;
	void Close(const ResultT&) override;

private:
	void OnStagePhaseChanged(const logic::StagePhase& stagePhase);
	void OnComponentsAnimationFinished();
	void MakeUIComponents();

private:
	ILogicGameplayFactory& m_logicFactory;
	IUIGameplayFactory& m_uiGameplayFactory;
	utils::unique_ref<IStageLogic> m_stageLogic;
	std::vector<utils::Connection> m_connections;
	std::vector<UIComponentType> m_componentTypes;
	bool m_canProcessInput;
};