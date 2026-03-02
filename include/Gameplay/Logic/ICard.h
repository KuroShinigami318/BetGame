#pragma once

#include "Components/IComponent.h"
#include "Components/IProbability.h"

class ICard : virtual public IComponent, public IProbability
{
public:
	virtual void SetMultiplier(const uint16_t&) = 0;
	virtual const uint16_t& GetMultiplierValue() const = 0;
	virtual void Bid(const uint32_t&) = 0;
	virtual void ResetBid(const uint32_t&) = 0;
	virtual const uint32_t& GetBid() const = 0;
	virtual std::optional<uint32_t> Roll() = 0;
};