#pragma once
#include "IInputExchanger.h"

template <typename T>
using ValueParserT = utils::CallableBound<T(const std::string&)>;
using InputFilterT = utils::CallableBound<bool(const char&)>;
template <typename T>
using InputParserT = utils::CallableBound<std::string(const T&)>;

template <typename T>
class InputExchangerTemplate : public IInputExchanger
{
public:
    InputExchangerTemplate(T& i_value, std::optional<T> i_initialValue = std::nullopt, ValueParserT<T> i_valueParser = nullptr, InputFilterT i_inputFilter = nullptr, InputParserT<T> i_inputParser = nullptr)
        : m_value(i_value)
        , m_valueParser(i_valueParser)
        , m_inputFilter(i_inputFilter)
        , m_inputParser(i_inputParser)
    {
        if (i_initialValue.has_value())
        {
            m_value = i_initialValue.value();
        }
    }

    void Exchange(const std::string& i_input) override
    {
        if (m_valueParser)
        {
            m_value = m_valueParser(i_input);
        }
    }

    bool Filter(const char& i_inputChar) const override
    {
        if (m_inputFilter)
        {
            return m_inputFilter(i_inputChar);
        }
        return true;
    }

    std::string GetCurrentInput() const override
    {
        if (m_inputParser)
        {
            return m_inputParser(m_value);
        }
        return {};
    }

    void RequestExchange() override
    {
        utils::Access<CallbackKey>(cb_onExchangeRequested).Emit();
    }

    void RequestValueUpdate() override
    {
        utils::Access<CallbackKey>(cb_onValueChanged).Emit();
    }

    void UpdateValue(const T& newValue)
    {
        m_value = newValue;
        utils::Access<CallbackKey>(cb_onValueChanged).Emit();
    }

private:
    T& m_value;
    ValueParserT<T> m_valueParser;
    InputFilterT m_inputFilter;
    InputParserT<T> m_inputParser;
};