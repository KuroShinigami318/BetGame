#include "stdafx.h"
#include "Components/Probability.h"

void Probability::SetRate(const float& i_rate)
{
	m_rate = i_rate;
}

const float& Probability::GetRate() const
{
	return m_rate;
}

utils::unique_ref<IComponent> Probability::Clone()
{
	utils::unique_ref<Probability> clone = utils::make_unique<Probability>();
	clone->SetRate(m_rate);
	return clone;
}

bool Probability::operator()() const
{
	return static_cast<float>(m_randomGenerator()) / static_cast<float>((m_randomGenerator.max)()) < m_rate;
}
