#pragma once

#include "IInputRelay.h"

class InputRelay : public IInputRelay
{
public:
	bool ProcessInput(const std::string& input) const override;
	void AddInputRelay(const IInputRelay& i_inputRelay);
	void RemoveInputRelay(const IInputRelay& i_inputRelay);
	void ClearInputRelays();

protected:
	virtual bool ProcessInputImpl(const std::string& input) const { return false; }

private:
	std::vector<const IInputRelay*> m_inputRelays;
};