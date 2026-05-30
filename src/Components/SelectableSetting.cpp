#include "stdafx.h"
#include "Components/SelectableSetting.h"
#include "Components/InputSelection.h"
#include "Components/LabelComponent.h"

SelectableSetting::SelectableSetting(const UIContext& i_uiContext, const std::string& i_text, const uint16_t& i_width, const uint16_t& i_height)
    : IUIComponent(i_uiContext)
    , SettingComponent(i_uiContext)
{
    Initialize<InputSelection, InputSelection, LabelComponent>(i_text, i_width, i_height, m_labelComponent, m_inputSelectionComponent);
    m_optionChangedConnection = m_inputSelectionComponent->sig_onOptionChanged.Connect(&SelectableSetting::OnOptionChanged, this);
}

void SelectableSetting::SetText(const std::string& i_text)
{
    m_labelComponent->SetText(i_text);
}

const std::string& SelectableSetting::GetText() const
{
    return m_labelComponent->GetText();
}

void SelectableSetting::AddOptions(std::initializer_list<utils::unique_ref<IOption>> i_options)
{
    m_inputSelectionComponent->AddOptions(i_options);
}

void SelectableSetting::AddOption(utils::unique_ref<IOption> i_option)
{
    m_inputSelectionComponent->AddOption(std::move(i_option));
}

void SelectableSetting::RemoveOption(IOption& i_option)
{
    m_inputSelectionComponent->RemoveOption(i_option);
}

void SelectableSetting::ClearOptions()
{
    m_inputSelectionComponent->ClearOptions();
}

void SelectableSetting::SetAsSelectedOption(IOption& i_option)
{
    m_inputSelectionComponent->SetAsSelectedOption(i_option);
}

IOption* SelectableSetting::GetSelectedOption() const
{
    return m_inputSelectionComponent->GetSelectedOption();
}

utils::unique_ref<IComponent> SelectableSetting::Clone()
{
    return utils::make_unique<SelectableSetting>(GetUIContext(), GetText(), GetWidth(), GetHeight());
}

void SelectableSetting::OnOptionChanged(IOption& i_option)
{
    utils::Access<SignalKey>(sig_onOptionChanged).Emit(i_option);
}