#pragma once

#include "IRenderStyle.h"
#include "Components/AttributesMap.h"
#include "Log.h"

class LabelRenderStyle : public IRenderStyle, public AttributesMap
{
public:
	void Render(RendererT&) const override;
	utils::unique_ref<IRenderStyle> Clone() const override;
};