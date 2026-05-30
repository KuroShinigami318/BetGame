#pragma once
#include "ILabel.h"

struct IInputExchanger;
enum class InputErrorType
{
    InvalidInput,
    ExchangeFailed,
    TextUpdateFailed
};

class IInputtableSetting : virtual public ILabel
{
protected:
    struct SignalKey;

public:
    virtual void SetInputExchanger(utils::unique_ref<IInputExchanger> i_inputExchanger) = 0;
    virtual IInputExchanger* GetInputExchanger() const = 0;

public:
    utils::Signal_public<void(InputErrorType), SignalKey> sig_onErrorEncountered;
    utils::Signal_public<void(), SignalKey> sig_onValueChanged;
};