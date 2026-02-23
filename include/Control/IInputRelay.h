#pragma once

class IInputRelay
{
public:
	virtual ~IInputRelay() = default;
	virtual bool ProcessInput(const std::string&) const = 0;
};