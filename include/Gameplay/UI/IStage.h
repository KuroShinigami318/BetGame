#pragma once

#include "UI/IUIComponent.h"

class IStage : public IUIComponent
{
public:
	using IUIComponent::IUIComponent;
	virtual void AddCardComponent(utils::unique_ref<IUIComponent>) = 0;
};