#pragma once
#include "IOption.h"

template <typename T>
class OptionTemplate : public IOption
{
public:
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    OptionTemplate(U&& i_value) : m_value(std::forward<U>(i_value))
    {
        m_text = utils::ConvertToString<char, T>(m_value);
    }

    const std::string& GetText() const override
    {
        return m_text;
    }

    T& GetValue() { return m_value; }
    const T& GetValue() const { return m_value; }

private:
    T m_value;
    std::string m_text;
};