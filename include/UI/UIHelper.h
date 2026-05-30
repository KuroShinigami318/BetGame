#pragma once
#include "Log.h"
#include "RendererType.h"

namespace ui
{
enum class TextAlignment : uint8_t;
}

class IUIComponent;
class IUIManager;
class IUIRenderStyleFactory;

constexpr const char* UI_RENDER_STYLE_FACTORY_ATTRIBUTE_KEY = "CurrentUIRenderStyleFactory";
constexpr const char* UI_RENDER_STYLE_NAME_ATTRIBUTE_KEY = "CurrentUIRenderStyleName";

template <typename T>
struct UniqueHolder
{
	UniqueHolder(utils::unique_ref<T> i_value)
		: value(std::move(i_value))
	{}
	UniqueHolder(const UniqueHolder& other)
		: value(std::move(const_cast<UniqueHolder&>(other).value))
	{}
	UniqueHolder& operator=(const UniqueHolder& other)
	{
		if (this != &other)
		{
			value = std::move(const_cast<UniqueHolder&>(other).value);
		}
		return *this;
	}
	utils::unique_ref<T> value;
};

std::string FormatText(const char* i_text, utils::Log::TextFormat i_textFormat);
void RenderTextWithAlignment(RendererT& o_renderer, const std::string& i_text, std::string::const_iterator& o_startIter, const ui::TextAlignment& i_textAlignment, uint16_t i_heightIndex, uint16_t i_width, uint16_t i_height);
void ApplyRenderStyle(IUIManager& uiManager, IUIRenderStyleFactory& factory);
void FocusUIComponent(const IUIComponent& uiComponent, const utils::RGBColor& i_focusColor = utils::RGBColor(0, 255, 0));
void UnFocusUIComponent(const IUIComponent& uiComponent);