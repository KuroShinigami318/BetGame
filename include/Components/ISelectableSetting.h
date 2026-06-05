#pragma once
#include "ILabel.h"

class IOption;

class ISelectableSetting : virtual public ILabel
{
protected:
    struct SignalKey;

public:
    virtual void AddOptions(std::initializer_list<utils::unique_ref<IOption>> i_options) = 0;
    virtual void AddOption(utils::unique_ref<IOption> i_option) = 0;
    virtual void RemoveOption(IOption& i_option) = 0;
    virtual void ClearOptions() = 0;
    virtual void SetAsSelectedOption(IOption& i_option) = 0;
    virtual IOption* GetSelectedOption() const = 0;

public:
    utils::Signal_public<void(IOption&), SignalKey> sig_onOptionChanged;
};