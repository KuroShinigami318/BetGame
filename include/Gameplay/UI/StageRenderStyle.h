#pragma once
#include "UI/IRenderStyle.h"
#include "Components/AttributesMap.h"

class StageRenderStyle : public IRenderStyle, public AttributesMap
{
private:
	using CardComponetsT = std::vector<utils::unique_ref<class IUIComponent>>;

public:
	StageRenderStyle(utils::unique_ref<IRenderStyle> i_cardRenderStyle);
	void Render(RendererT& o_renderStream) const override;
	void OnThisRenderStyleApplied() const override;
	void OnThisRenderStyleUnapplied() const override;
	utils::unique_ref<IRenderStyle> Clone() const override;

private:
	const CardComponetsT* GetCardComponents() const;

private:
	utils::unique_ref<IRenderStyle> m_cardRenderStyle;
	mutable utils::unique_ptr<IRenderStyle> m_previousCardRenderStyle;
};