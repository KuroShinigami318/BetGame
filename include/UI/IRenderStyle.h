#pragma once
#include "RendererType.h"
#include "Components/IAttributesMap.h"

class IRenderStyle : virtual public IAttributesMap
{
public:
	virtual ~IRenderStyle() = default;
	virtual void Render(RendererT&) const = 0;
	virtual utils::unique_ref<IRenderStyle> Clone() const = 0;

public:
	virtual void OnThisRenderStyleApplied() const {};
	virtual void OnThisRenderStyleUnapplied() const {};
};