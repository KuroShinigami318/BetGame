#include "stdafx.h"
#include "UI/StartGameFlow.h"
#include "Components/Text.h"
#include "Control/ActionCode.h"
#include "ICommandManager.h"
#include "UI/IUIManager.h"
#include "UI/IWindow.h"
#include "UI/Screens/GameplayWindow.h"
#include "Gameplay/Logic/BasicLogicGameplayFactory.h"
#include "Gameplay/Logic/LogicConfig.h"
#include "Gameplay/UI/BasicUIGameplayFactory.h"
#include "Gameplay/UI/BasicUIRenderStyleFactory.h"
#include "Gameplay/UI/ApplyRenderStyleCommand.h"
#include "AsyncScopedHelper.h"
#include "ThreadControl.h"
#include "system_clock.h"

StartGameFlow::StartGameFlow(IWindow& i_splashscreenWindow, ICommandManager& i_commandManager)
	: m_splashscreenWindow(i_splashscreenWindow)
	, m_commandManager(i_commandManager)
	, m_isEnterPressed(false)
	, m_asyncScopedHelper(utils::make_unique<utils::AsyncScopedHelper>())
{
	m_asyncScopedHelper->StartOptionalTask(m_splashscreenWindow.GetUIContext().nextFrameQueue, &StartGameFlow::StartGame, this);
}

bool StartGameFlow::ProcessInput(const std::string& i_input) const
{
	return m_isEnterPressed = m_splashscreenWindow.GetUIContext().uiManager.IsInputAction(i_input, ActionCode::Enter);
}

void StartGameFlow::StartGame()
{
	BasicLogicGameplayFactory logicFactory(utils::make_unique<LogicConfig>());
	BasicUIGameplayFactory uiFactory;
	const UIContext& uiContext = m_splashscreenWindow.GetUIContext();
	utils::SystemClock systemClock;
	utils::RecursiveYielder recursiveYielder(uiContext.nextFrameQueue, uiContext.recursiveControl, systemClock);
	IUIComponent& textRef = m_splashscreenWindow.AddUIComponent(utils::make_unique<Text>(uiContext, "Press [Enter] to continue!"));
	m_isEnterPressed = false;
	while (!m_isEnterPressed)
	{
		if (!m_splashscreenWindow.IsOpened() || recursiveYielder.DoYieldWithResult(utils::IYielder::Mode::Forced) == utils::IRecursiveControl::ErrorCode::ShuttingDown)
		{
			return;
		}
	}

	m_splashscreenWindow.RetrieveUIComponent(textRef).assertSuccess();
	GameplayWindow gameplayWindow(uiContext, logicFactory, uiFactory);
	m_commandManager.ExecuteAndForget(utils::make_unique<ApplyRenderStyleCommand>(uiContext.uiManager, utils::make_unique<BasicUIRenderStyleFactory>()));
	gameplayWindow.Open();
	m_asyncScopedHelper->StartOptionalTask(m_splashscreenWindow.GetUIContext().nextFrameQueue, &StartGameFlow::StartGame, this);
}
