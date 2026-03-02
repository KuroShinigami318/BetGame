#include "stdafx.h"
#include "Gameplay/UI/BasicUIGameplayFactory.h"
#include "Gameplay/UI/UIComponentsEnum.h"
#include "DisplayInfo.h"
#include "Gameplay/UI/BidComponent.h"
#include "Gameplay/UI/CardComponent.h"
#include "Gameplay/UI/Stage.h"
#include "UI/IUIManager.h"

namespace
{
constexpr const uint16_t CARD_COMPONENT_WIDTH = 20;
constexpr const uint16_t CARD_COMPONENT_HEIGHT = 6;
constexpr const uint16_t BID_COMPONENT_HEIGHT = 3;
constexpr const size_t TOTAL_UI_COMPONENTS = 2;
}

std::vector<IUIGameplayFactory::UIHolder> BasicUIGameplayFactory::CreateUIComponents(const UIContext& i_uiContext, IStageLogic& i_stageLogic)
{
	std::vector<IUIGameplayFactory::UIHolder> uiHolders;
	uiHolders.reserve(TOTAL_UI_COMPONENTS);
	uiHolders.push_back({ UIComponentType::Bid, CreateBidComponent(i_uiContext, i_stageLogic) });
	uiHolders.push_back({ UIComponentType::Stage, CreateStageComponent(i_uiContext, i_stageLogic) });
	return uiHolders;
}

std::string BasicUIGameplayFactory::GetName() const
{
	return GetStaticName();
}

std::string BasicUIGameplayFactory::GetStaticName()
{
	return "BasicUIGameplayFactory";
}

utils::unique_ref<IStage> BasicUIGameplayFactory::CreateStageComponent(const UIContext& i_uiContext, IStageLogic& i_stageLogic)
{
	const DisplayInfo& displayInfo = i_uiContext.uiManager.GetDisplayInfo();
	utils::unique_ref<Stage> stage = utils::make_unique<Stage>(i_uiContext, i_stageLogic, displayInfo.width, displayInfo.height - BID_COMPONENT_HEIGHT);
	uint16_t totalCards = m_randomGenerator();
	for (uint16_t cardIndex = 0; cardIndex < totalCards; ++cardIndex)
	{
		utils::unique_ref<CardComponent> cardComponent = utils::make_unique<CardComponent>(i_uiContext, cardIndex + 1, CARD_COMPONENT_WIDTH, CARD_COMPONENT_HEIGHT);
		ICard& cardComponentRef = *cardComponent;
		stage->AddCardComponent(std::move(cardComponent));
		i_stageLogic.AddCardComponent(cardComponentRef);
	}
	return std::move(stage);
}

utils::unique_ref<IBid> BasicUIGameplayFactory::CreateBidComponent(const UIContext& i_uiContext, IStageLogic& i_stageLogic)
{
	const DisplayInfo& displayInfo = i_uiContext.uiManager.GetDisplayInfo();
	return utils::make_unique<BidComponent>(i_uiContext, i_stageLogic, displayInfo.width, BID_COMPONENT_HEIGHT);
}