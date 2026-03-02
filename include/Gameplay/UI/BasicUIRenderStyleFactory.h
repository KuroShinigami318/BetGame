#pragma once

#include "Gameplay/UI/IUIRenderStyleFactory.h"

class BasicUIRenderStyleFactory : public IUIRenderStyleFactory
{
public:
	std::vector<UIRenderStyleHolder> CreateUIRenderStyles() override;
	std::string GetName() const override;
	static std::string GetStaticName();

private:
	utils::unique_ref<IRenderStyle> CreateBidRenderStyle();
	utils::unique_ref<IRenderStyle> CreateCardRenderStyle();
	utils::unique_ref<IRenderStyle> CreateStageRenderStyle();
};