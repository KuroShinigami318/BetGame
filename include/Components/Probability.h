#pragma once

#include "IComponent.h"
#include "IProbability.h"
#include "random_generator.h"

class Probability : public IProbability, virtual public IComponent
{
public:
	void SetRate(const float& i_rate) override;
	const float& GetRate() const override;
	utils::unique_ref<IComponent> Clone() override;
	bool operator()() const override;

private:
	float m_rate = 0.f;
	mutable utils::RandomGenerator<uint32_t, 0, (std::numeric_limits<uint32_t>::max)() - 1> m_randomGenerator;
};