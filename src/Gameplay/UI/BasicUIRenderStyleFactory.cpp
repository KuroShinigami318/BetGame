#include "stdafx.h"
#include "Gameplay/UI/BasicUIRenderStyleFactory.h"
#include "UI/IRenderStyle.h"
#include "Gameplay/UI/BidRenderStyle.h"
#include "Gameplay/UI/CardRenderStyle.h"
#include "Gameplay/UI/StageRenderStyle.h"
#include "Gameplay/UI/UIComponentsEnum.h"

namespace
{
constexpr const size_t TOTAL_UI_RENDER_STYLES = 2;
}

std::vector<IUIRenderStyleFactory::UIRenderStyleHolder> BasicUIRenderStyleFactory::CreateUIRenderStyles()
{
	std::vector<UIRenderStyleHolder> renderStyles;
	renderStyles.reserve(TOTAL_UI_RENDER_STYLES);
	renderStyles.push_back({ UIComponentType::Bid, CreateBidRenderStyle() });
	renderStyles.push_back({ UIComponentType::Stage, CreateStageRenderStyle() });
	return renderStyles;
}

std::string BasicUIRenderStyleFactory::GetName() const
{
	return GetStaticName();
}

std::string BasicUIRenderStyleFactory::GetStaticName()
{
	return "BasicUIRenderStyleFactory";
}

utils::unique_ref<IRenderStyle> BasicUIRenderStyleFactory::CreateBidRenderStyle()
{
	return utils::make_unique<BidRenderStyle>();
}

utils::unique_ref<IRenderStyle> BasicUIRenderStyleFactory::CreateCardRenderStyle()
{
	return utils::make_unique<CardRenderStyle>();
}

utils::unique_ref<IRenderStyle> BasicUIRenderStyleFactory::CreateStageRenderStyle()
{
	return utils::make_unique<StageRenderStyle>(CreateCardRenderStyle());
}