#include "stdafx.h"
#include "Components/LabelComponent.h"
#include "UI/LabelRenderStyle.h"
#include "UI/UIHelper.h"
#include "attribute.h"

LabelComponent::LabelComponent(const UIContext& i_uiContext, const std::string& i_text, const uint16_t& i_width, const uint16_t& i_height)
    : IUIComponent(i_uiContext)
    , m_width(i_width)
    , m_height(i_height)
    , m_text(i_text)
{
    SetRenderStyle(utils::make_unique<LabelRenderStyle>());
}

void LabelComponent::SetText(const std::string& i_text)
{
    m_text = i_text;
}

const std::string& LabelComponent::GetText() const
{
    return m_text;
}

void LabelComponent::SetWidth(const uint16_t& i_width)
{
    m_width = i_width;
}

void LabelComponent::SetHeight(const uint16_t& i_height)
{
    m_height = i_height;
}

uint16_t LabelComponent::GetWidth() const
{
    return m_width;
}

uint16_t LabelComponent::GetHeight() const
{
    return m_height;
}

void LabelComponent::OnFocusGained(const utils::RGBColor& i_focusColor)
{
    FocusUIComponent(*this, i_focusColor);
}

void LabelComponent::OnFocusLost()
{
    UnFocusUIComponent(*this);
}

void LabelComponent::SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle)
{
    i_renderStyle->BindAttribute("text", utils::attribute::make_bind_attribute(m_text));
    i_renderStyle->BindAttribute("width", utils::attribute::make_bind_attribute(m_width));
    i_renderStyle->BindAttribute("height", utils::attribute::make_bind_attribute(m_height));
    IUIComponent::SetRenderStyle(std::move(i_renderStyle));
}

void LabelComponent::Render(RendererT& o_renderStream) const
{
    m_renderStyle->Render(o_renderStream);
    o_renderStream << utils::Format("\033[{}B", m_height); // Move cursor to the bottom of the rendered component
}

utils::unique_ref<IComponent> LabelComponent::Clone()
{
    return utils::make_unique<LabelComponent>(GetUIContext(), m_text, m_width, m_height);
}