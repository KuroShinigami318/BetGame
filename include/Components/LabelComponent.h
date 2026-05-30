#pragma once
#include "ILabel.h"
#include "UI/IUIComponent.h"

class LabelComponent : virtual public ILabel, virtual public IUIComponent
{
public:
    LabelComponent(const UIContext& i_uiContext, const std::string& i_text = "", const uint16_t& i_width = 0, const uint16_t& i_height = 0);
    void SetText(const std::string& i_text) override;
    const std::string& GetText() const override;
    void SetWidth(const uint16_t& i_width) override;
    void SetHeight(const uint16_t& i_height) override;
    uint16_t GetWidth() const override;
    uint16_t GetHeight() const override;
    void OnFocusGained(const utils::RGBColor& i_focusColor) override;
    void OnFocusLost() override;
    void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) override;
    void Render(RendererT& o_renderStream) const override;
    utils::unique_ref<IComponent> Clone() override;

private:
    uint16_t m_width;
    uint16_t m_height;
    std::string m_text;
};