#include "stdafx.h"
#include "Components/Text.h"

Text::Text(const UIContext& i_uiContext, const std::string& i_text)
	: IUIComponent(i_uiContext)
	, m_text(i_text)
{
}

utils::unique_ref<IComponent> Text::Clone()
{
	return utils::make_unique<Text>(m_uiContext, m_text);
}

void Text::Render(RendererT& o_renderStream) const
{
	o_renderStream << m_text;
	o_renderStream << std::endl << "\033[1G";
}
