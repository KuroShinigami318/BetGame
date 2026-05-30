#pragma once

#include "IComponent.h"

class ITextInput : virtual public IComponent
{
protected:
    struct SignalKey;

public:
    virtual void SetText(const std::string& i_text) = 0;
    virtual const std::string& GetText() const = 0;

public:
    utils::Signal_public<void(const std::string&), SignalKey> sig_onTextChanged;
    utils::Callback_public<bool(const std::string&), SignalKey> sig_onTextFiltered;
};