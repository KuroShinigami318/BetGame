#pragma once
#include "IInputtableSetting.h"
#include "SettingComponent.h"

class ITextInput;

class InputtableSetting : public IInputtableSetting, private SettingComponent, virtual public IUIComponent, virtual public IInputRelay
{
public:
    InputtableSetting(const UIContext& i_uiContext, const std::string& i_text = "", const uint16_t& i_width = 0, const uint16_t& i_height = 0);
    void SetInputExchanger(utils::unique_ref<IInputExchanger> i_inputExchanger) override;
    IInputExchanger* GetInputExchanger() const override;
    void SetText(const std::string& i_text) override;
    const std::string& GetText() const override;
    utils::unique_ref<IComponent> Clone() override;

private:
    bool OnTextFiltered(const std::string& i_input);
    void OnTextChanged(const std::string& i_input);
    void OnInputExchangerValueChanged();
    void OnInputExchangerExchangeRequested();

private:
    ILabel* m_labelComponent;
    ITextInput* m_textInput;
    utils::unique_ptr<IInputExchanger> m_inputExchanger;
    std::vector<utils::Connection> m_signalConnections;
};