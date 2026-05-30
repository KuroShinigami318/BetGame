#pragma once
#include "UI/IUIComponent.h"
#include "Control/IInputRelay.h"
#include <initializer_list>

class IOption;
namespace utils
{
class intrusive_list;
}

class InputSelection : virtual public IUIComponent, virtual public IInputRelay, utils::noncopy, utils::nonmove
{
protected:
    struct SignalKey;

public:
    ~InputSelection();
    InputSelection(const UIContext& i_uiContext, const uint16_t& i_width = 0, const uint16_t& i_height = 0);
    void SetWidth(const uint16_t& i_width) override;
    void SetHeight(const uint16_t& i_height) override;
    uint16_t GetWidth() const override;
    uint16_t GetHeight() const override;
    void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) override;
    void Render(RendererT& o_renderStream) const override;
    void OnFocusGained(const utils::RGBColor& i_focusColor) override;
    void OnFocusLost() override;
    bool ProcessInput(const std::string& i_input) const override;
    virtual void AddOptions(std::initializer_list<utils::unique_ref<IOption>> i_options);
    virtual void AddOption(utils::unique_ref<IOption> i_option);
    virtual void RemoveOption(IOption& i_option);
    virtual void ClearOptions();
    void SetAsSelectedOption(IOption& i_option);
    IOption* GetSelectedOption() const;
    utils::unique_ref<IComponent> Clone() override;

private:
    void SwitchToNextOption();
    void SwitchToPreviousOption();

public:
    utils::Signal_public<void(IOption&), SignalKey> sig_onOptionChanged;

private:
    uint16_t m_width;
    uint16_t m_height;
    utils::unique_ref<IOption> m_listEndOption;
    utils::unique_ref<utils::intrusive_list> m_optionsList;
    IOption* m_selectedOption;
};