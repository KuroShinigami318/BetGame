#include "UI/Screens/GameplayWindow.h"
#include "UI/IUIManager.h"
#include "UI/UIHelper.h"
#include "Gameplay/Logic/IStageLogic.h"
#include "Gameplay/Logic/ILogicGameplayFactory.h"
#include "Gameplay/Logic/StagePhase.h"
#include "Gameplay/UI/IUIGameplayFactory.h"

#include "attribute.h"

GameplayWindow::GameplayWindow(const UIContext& i_uiContext, ILogicGameplayFactory& i_logicFactory, IUIGameplayFactory& i_uiGameplayFactory)
	: WindowBase(i_uiContext)
	, m_logicFactory(i_logicFactory)
	, m_uiGameplayFactory(i_uiGameplayFactory)
	, m_stageLogic(i_logicFactory.CreateStageLogic())
	, m_canProcessInput(true)
{
	MakeUIComponents();
	m_stageLogic->Reset();
	m_connections.push_back(m_stageLogic->sig_onStagePhaseChanged.Connect(&GameplayWindow::OnStagePhaseChanged, this));
	m_connections.push_back(m_stageLogic->sig_onAnimationFinished.Connect(&GameplayWindow::OnComponentsAnimationFinished, this));
}

GameplayWindow::~GameplayWindow()
{
	while (!m_componentTypes.empty())
	{
		m_uiContext.uiManager.UnmapUIComponent(m_componentTypes.back()).assertSuccess();
		m_componentTypes.pop_back();
	}
}

bool GameplayWindow::ProcessInput(const std::string& input) const
{
	return m_canProcessInput ? WindowBase::ProcessInput(input) : false;
}

utils::unique_ref<IComponent> GameplayWindow::Clone()
{
	return utils::make_unique<GameplayWindow>(m_uiContext, m_logicFactory, m_uiGameplayFactory);
}

void GameplayWindow::Close()
{
	while (!m_componentTypes.empty())
	{
		m_uiContext.uiManager.UnmapUIComponent(m_componentTypes.back()).assertSuccess();
		m_componentTypes.pop_back();
	}
	WindowBase::Close();
}

void GameplayWindow::OnStagePhaseChanged(const logic::StagePhase& stagePhase)
{
	if (stagePhase == logic::StagePhase::RollStarted)
	{
		m_canProcessInput = false;
	}
}

void GameplayWindow::OnComponentsAnimationFinished()
{
	m_canProcessInput = true;
}

void GameplayWindow::MakeUIComponents()
{
	std::vector<IUIGameplayFactory::UIHolder> uiHolders = m_uiGameplayFactory.CreateUIComponents(m_uiContext, *m_stageLogic);
	for (auto& uiHolder : uiHolders)
	{
		IUIManager::ResultT mapResult = m_uiContext.uiManager.MapUIComponent(AddUIComponent(std::move(uiHolder.uiComponent)), uiHolder.componentType);
		mapResult.assertSuccess();
		if (mapResult.isOk())
		{
			m_componentTypes.push_back(uiHolder.componentType);
		}
	}

	utils::attribute* renderStyleFactoryAttribute = m_uiContext.uiManager.GetAttribute(UI_RENDER_STYLE_FACTORY_ATTRIBUTE_KEY);
	if (renderStyleFactoryAttribute)
	{
		const UniqueHolder<IUIRenderStyleFactory>* renderStyleFactory = nullptr;
		renderStyleFactoryAttribute->get(renderStyleFactory).ignoreResult();
		if (renderStyleFactory && !renderStyleFactory->value.valueless_after_move())
		{
			ApplyRenderStyle(m_uiContext.uiManager, *renderStyleFactory->value);
		}
	}
}
