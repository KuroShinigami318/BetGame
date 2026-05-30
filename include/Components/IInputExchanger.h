#pragma once

struct IInputExchanger
{
protected:
    struct CallbackKey;

public:
    virtual ~IInputExchanger() = default;
    virtual void Exchange(const std::string& i_input) = 0;
    virtual bool Filter(const char& i_inputChar) const = 0;
    virtual std::string GetCurrentInput() const = 0;
    virtual void RequestExchange() = 0;
    virtual void RequestValueUpdate() = 0;

public:
    utils::Callback_public<void(), CallbackKey> cb_onValueChanged;
    utils::Callback_public<void(), CallbackKey> cb_onExchangeRequested;
};