#pragma once
#include "UI/IRenderStyle.h"
#include "Components/AttributesMap.h"

class CardRenderStyle : public IRenderStyle, public AttributesMap
{
public:
	CardRenderStyle();
	CardRenderStyle(const CardRenderStyle&);
	CardRenderStyle& operator=(const CardRenderStyle&);
	void OnThisRenderStyleApplied() const override;
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IRenderStyle> Clone() const override;

private:
	utils::unique_ref<IRenderStyle> m_titleRenderStyle;
	utils::unique_ref<IRenderStyle> m_bodyRenderStyle;
};