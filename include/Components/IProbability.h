#pragma once

class IProbability
{
public:
	virtual ~IProbability() = default;
	// rate in range [0, 1]
	virtual void SetRate(const float&) = 0;
	virtual const float& GetRate() const = 0;
	virtual bool operator()() const = 0;
};