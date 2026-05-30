#include "stdafx.h"
#include "Components/InputtableSetting.h"
#include "Components/IInputExchanger.h"
#include "Components/LabelComponent.h"
#include "Components/TextInput.h"

InputtableSetting::InputtableSetting(const UIContext& i_uiContext, const std::string& i_text, const uint16_t& i_width, const uint16_t& i_height)
    : IUIComponent(i_uiContext)
    , SettingComponent(i_uiContext)
{
    Initialize<TextInput, ITextInput, LabelComponent>(i_text, i_width, i_height, m_labelComponent, m_textInput);
}

void InputtableSetting::SetInputExchanger(utils::unique_ref<IInputExchanger> i_inputExchanger)
{
    m_signalConnections.clear();
    m_inputExchanger = std::move(i_inputExchanger);
    m_signalConnections.push_back(m_textInput->sig_onTextFiltered.Connect(&InputtableSetting::OnTextFiltered, this));
    m_signalConnections.push_back(m_textInput->sig_onTextChanged.Connect(&InputtableSetting::OnTextChanged, this));
    m_signalConnections.push_back(m_inputExchanger->cb_onValueChanged.Connect(&InputtableSetting::OnInputExchangerValueChanged, this));
    m_signalConnections.push_back(m_inputExchanger->cb_onExchangeRequested.Connect(&InputtableSetting::OnInputExchangerExchangeRequested, this));
    m_inputExchanger->RequestValueUpdate();
}

IInputExchanger* InputtableSetting::GetInputExchanger() const
{
    return m_inputExchanger.get();
}

void InputtableSetting::SetText(const std::string& i_text)
{
    m_labelComponent->SetText(i_text);
}

const std::string& InputtableSetting::GetText() const
{
    return m_labelComponent->GetText();
}

utils::unique_ref<IComponent> InputtableSetting::Clone()
{
    return utils::make_unique<InputtableSetting>(GetUIContext(), GetText(), GetWidth(), GetHeight());
}

bool InputtableSetting::OnTextFiltered(const std::string& i_input)
{
    for (const char& inputChar : i_input)
    {
        if (!m_inputExchanger->Filter(inputChar))
        {
            utils::Access<IInputtableSetting::SignalKey>(sig_onErrorEncountered).Emit(InputErrorType::InvalidInput);
            return false;
        }
    }
    return true;
}

void InputtableSetting::OnTextChanged(const std::string& i_input)
{
    try
    {
        m_inputExchanger->Exchange(i_input);
        utils::Access<IInputtableSetting::SignalKey>(sig_onValueChanged).Emit();
    }
    catch (...)
    {
        utils::Access<IInputtableSetting::SignalKey>(sig_onErrorEncountered).Emit(InputErrorType::ExchangeFailed);
    }
}

void InputtableSetting::OnInputExchangerValueChanged()
{
    try
    {
        m_textInput->SetText(m_inputExchanger->GetCurrentInput());
    }
    catch (...)
    {
        utils::Access<IInputtableSetting::SignalKey>(sig_onErrorEncountered).Emit(InputErrorType::TextUpdateFailed);
    }
}

void InputtableSetting::OnInputExchangerExchangeRequested()
{
    try
    {
        m_inputExchanger->Exchange(m_textInput->GetText());
        utils::Access<IInputtableSetting::SignalKey>(sig_onValueChanged).Emit();
    }
    catch (...)
    {
        utils::Access<IInputtableSetting::SignalKey>(sig_onErrorEncountered).Emit(InputErrorType::ExchangeFailed);
    }
}