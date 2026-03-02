#pragma once

#include "UI/IUIComponent.h"

class IBid : public IUIComponent
{
public:
	using IUIComponent::IUIComponent;
	virtual void SetBid(const int&) = 0;
	virtual const int& GetBid() const = 0;
};