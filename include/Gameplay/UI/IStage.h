#pragma once

#include "UI/IUIComponent.h"

class IStage : public IUIComponent
{
public:
	using IUIComponent::IUIComponent;
	virtual void ReserveCardComponentCount(const size_t& i_count) = 0;
	virtual void SetCardComponent(utils::unique_ref<IUIComponent> i_card, const size_t& i_index) = 0;
};