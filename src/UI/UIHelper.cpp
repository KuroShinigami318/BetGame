#include "stdafx.h"
#include "UI/UIHelper.h"
#include "UI/TextAlignment.h"
#include "UI/IUIComponent.h"
#include "UI/IUIManager.h"
#include "Gameplay/UI/IUIRenderStyleFactory.h"

std::string FormatText(const char* i_text, utils::Log::TextFormat i_textFormat)
{
	ScopedVirtualConsoleMode();
	std::string formatterColor = "\033[{0}m{1}\033[0m";
	if (i_textFormat.textColor.IsValidColor() && i_textFormat.backgroundColor.IsValidColor())
	{
		formatterColor = "\033[{0};38;2;{2};{3};{4};48;2;{5};{6};{7}m{1}\033[0m";
	}
	else if (i_textFormat.textColor.IsValidColor())
	{
		formatterColor = "\033[{0};38;2;{2};{3};{4}m{1}\033[0m";
	}
	else if (i_textFormat.backgroundColor.IsValidColor())
	{
		formatterColor = "\033[{0};48;2;{5};{6};{7}m{1}\033[0m";
	}
	return utils::Format(formatterColor.c_str(), std::underlying_type_t<utils::Log::TextStyle>(i_textFormat.textStyle), i_text, i_textFormat.textColor.red.value, i_textFormat.textColor.green.value, i_textFormat.textColor.blue.value, i_textFormat.backgroundColor.red.value, i_textFormat.backgroundColor.green.value, i_textFormat.backgroundColor.blue.value);
}

void RenderTextWithAlignment(RendererT& o_renderer, const std::string& i_text, std::string::const_iterator& o_startIter, const ui::TextAlignment& i_textAlignment, uint16_t i_heightIndex, uint16_t i_width, uint16_t i_height)
{

}

void ApplyRenderStyle(IUIManager& uiManager, IUIRenderStyleFactory& factory)
{
	for (auto& renderStyleHolder : factory.CreateUIRenderStyles())
	{
		auto uiComponentResult = uiManager.GetUIComponent(renderStyleHolder.componentType);
		if (uiComponentResult.isErr())
		{
			continue;
		}
		IUIComponent* uiComponent = const_cast<IUIComponent*>(uiComponentResult.unwrap());
		uiComponent->SetRenderStyle(std::move(renderStyleHolder.uiRenderStyle));
	}
}
