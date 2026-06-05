#pragma once

enum class ActionCode : uint8_t;

class IInputHints
{
public:
    virtual ~IInputHints() = default;
    virtual void AddHint(ActionCode i_actionCode, const std::string& i_hint, utils::CallableBound<bool(const std::string&)> i_processInputFunc = nullptr) = 0;
};