#pragma once
#include "ITextInput.h"
#include "UI/IUIComponent.h"
#include "Control/IInputRelay.h"

class TextInput : public ITextInput, public IUIComponent, public IInputRelay
{
public:
    TextInput(const UIContext& i_uiContext, uint16_t i_width = 0, uint16_t i_height = 0);
    utils::unique_ref<IComponent> Clone() override;
    void Render(RendererT& o_renderStream) const override;
    void SetWidth(const uint16_t& i_width) override;
    void SetHeight(const uint16_t& i_height) override;
    uint16_t GetWidth() const override;
    uint16_t GetHeight() const override;
    void SetText(const std::string& i_text) override;
    const std::string& GetText() const override;
    void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) override;
    void OnFocusGained(const utils::RGBColor& i_focusColor) override;
    void OnFocusLost() override;
    bool ProcessInput(const std::string& i_input) const override;
    void InitializeInputHints(IInputHints& i_inputHints) const override;

private:
    uint16_t m_width;
    uint16_t m_height;
    std::string m_inputText;
};