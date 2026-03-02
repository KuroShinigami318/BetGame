#pragma once
#include "UI/IRenderStyle.h"
#include "Components/AttributesMap.h"

class BidRenderStyle : public IRenderStyle, public AttributesMap
{
public:
	BidRenderStyle();
	BidRenderStyle(const BidRenderStyle& other);
	BidRenderStyle& operator=(const BidRenderStyle& other);
	void OnThisRenderStyleApplied() const override;
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IRenderStyle> Clone() const override;

private:
	utils::unique_ref<IRenderStyle> m_labelRenderStyle;
};