#include "stdafx.h"
#include "UI/StartGameFlow.h"
#include "Components/Text.h"
#include "Control/ActionCode.h"
#include "ICommandManager.h"
#include "UI/IUIManager.h"
#include "UI/IWindow.h"
#include "UI/Screens/MainMenuWindow.h"
#include "UI/Screens/GameplayWindow.h"
#include "Gameplay/Logic/BasicLogicGameplayFactory.h"
#include "Gameplay/Logic/LogicConfig.h"
#include "Gameplay/UI/BasicUIGameplayFactory.h"
#include "Gameplay/UI/BasicUIRenderStyleFactory.h"
#include "Gameplay/UI/ApplyRenderStyleCommand.h"
#include "AsyncScopedHelper.h"

StartGameFlow::StartGameFlow(IWindow& i_splashscreenWindow)
	: m_splashscreenWindow(i_splashscreenWindow)
	, m_asyncScopedHelper(utils::make_unique<utils::AsyncScopedHelper>())
{
	m_asyncScopedHelper->StartOptionalTask(m_splashscreenWindow.GetUIContext().nextFrameQueue, &StartGameFlow::StartGame, this);
}

void StartGameFlow::StartGame()
{
	utils::unique_ref<LogicConfig> logicConfig = utils::make_unique<LogicConfig>();
	LogicConfig& logicConfigRef = *logicConfig;
	BasicLogicGameplayFactory logicFactory(std::move(logicConfig));
	BasicUIGameplayFactory uiFactory;
	const UIContext& uiContext = m_splashscreenWindow.GetUIContext();

	MainMenuWindow mainMenuWindow(uiContext, logicConfigRef);
	IWindow::ResultT exitGameResult = utils::make_any<ExitGameActionTag>();
	const IWindow::ResultT* openMainMenuResult = nullptr;
	do
	{
		auto openWindowResult = mainMenuWindow.Open();
		if (openWindowResult.isErr())
		{
			openMainMenuResult = &exitGameResult;
			break;
		}
		if (utils::any_cast<StartGameActionTag>(openMainMenuResult = openWindowResult.unwrap()))
		{
			GameplayWindow gameplayWindow(uiContext, logicFactory, uiFactory);
			uiContext.commandManager.ExecuteAndForget(utils::make_unique<ApplyRenderStyleCommand>(uiContext.uiManager, utils::make_unique<BasicUIRenderStyleFactory>()));
			openWindowResult = gameplayWindow.Open();
			if (openWindowResult.isErr())
			{
				openMainMenuResult = &exitGameResult;
				break;
			}
			if (utils::any_cast<ClosedByExitFlowTag>(openWindowResult.unwrap()))
			{
				return;
			}
		}
	} while (!(utils::any_cast<ExitGameActionTag>(openMainMenuResult) || utils::any_cast<ClosedByExitFlowTag>(openMainMenuResult)));
	if (utils::any_cast<ExitGameActionTag>(openMainMenuResult))
	{
		m_splashscreenWindow.Close(ClosedByExitFlowTag{});
	}
}
