#pragma once
#include "ILabel.h"

class IButton : virtual public ILabel
{
protected:
    struct SignalKey;

public:
    virtual ~IButton() = default;
    utils::Signal_public<void(), SignalKey> sig_onAction;
};