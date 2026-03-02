#pragma once
#include "UI/IUIComponent.h"

class Text : public IUIComponent
{
public:
	Text(const UIContext& i_uiContext, const std::string& i_text);
	virtual utils::unique_ref<IComponent> Clone() override;
	virtual void Render(RendererT& o_renderStream) const override;

private:
	std::string m_text;
};