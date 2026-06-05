#pragma once

class IInputHints;

class IInputRelay
{
public:
	virtual ~IInputRelay() = default;
	virtual bool ProcessInput(const std::string&) const = 0;
	virtual void InitializeInputHints(IInputHints& i_inputHints) const {}
};