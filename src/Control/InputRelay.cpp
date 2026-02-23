#include "stdafx.h"
#include "Control/InputRelay.h"

bool InputRelay::ProcessInput(const std::string& input) const
{
	const bool processed = ProcessInputImpl(input);
	if (!processed)
	{
		for (const auto& inputRelay : m_inputRelays)
		{
			if (inputRelay->ProcessInput(input))
			{
				return true;
			}
		}
	}
	return processed;
}

void InputRelay::AddInputRelay(const IInputRelay& i_inputRelay)
{
	auto inputRelayFoundIt = std::find_if(m_inputRelays.cbegin(), m_inputRelays.cend(), [&i_inputRelay](const IInputRelay* inputRelay)
	{
		return inputRelay == &i_inputRelay;
	});
	if (inputRelayFoundIt == m_inputRelays.end())
	{
		m_inputRelays.push_back(&i_inputRelay);
	}
}

void InputRelay::RemoveInputRelay(const IInputRelay& i_inputRelay)
{
	auto inputRelayFoundIt = std::find_if(m_inputRelays.cbegin(), m_inputRelays.cend(), [&i_inputRelay](const IInputRelay* inputRelay)
	{
		return inputRelay == &i_inputRelay;
	});
	if (inputRelayFoundIt != m_inputRelays.end())
	{
		m_inputRelays.erase(inputRelayFoundIt);
	}
}

void InputRelay::ClearInputRelays()
{
	m_inputRelays.clear();
}