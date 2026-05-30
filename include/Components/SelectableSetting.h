#pragma once
#include "ISelectableSetting.h"
#include "SettingComponent.h"

class InputSelection;

class SelectableSetting : public ISelectableSetting, private SettingComponent, virtual public IUIComponent, virtual public IInputRelay
{
public:
    SelectableSetting(const UIContext& i_uiContext, const std::string& i_text = "", const uint16_t& i_width = 0, const uint16_t& i_height = 0);
    void SetText(const std::string& i_text) override;
    const std::string& GetText() const override;
    void AddOptions(std::initializer_list<utils::unique_ref<IOption>> i_options) override;
    void AddOption(utils::unique_ref<IOption> i_option) override;
    void RemoveOption(IOption& i_option) override;
    void ClearOptions() override;
    void SetAsSelectedOption(IOption& i_option) override;
    IOption* GetSelectedOption() const override;
    utils::unique_ref<IComponent> Clone() override;

private:
    void OnOptionChanged(IOption& i_option);

private:
    ILabel* m_labelComponent;
    InputSelection* m_inputSelectionComponent;
    utils::Connection m_optionChangedConnection;
};