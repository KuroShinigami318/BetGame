#include "stdafx.h"
#include "Components/TextInput.h"
#include "Control/ActionCode.h"
#include "UI/LabelRenderStyle.h"
#include "UI/IInputHints.h"
#include "UI/IUIManager.h"
#include "UI/UIHelper.h"

#include "attribute.h"

TextInput::TextInput(const UIContext& i_uiContext, uint16_t i_width, uint16_t i_height)
    : IUIComponent(i_uiContext), m_width(i_width), m_height(i_height)
{
    SetRenderStyle(utils::make_unique<LabelRenderStyle>());
}

utils::unique_ref<IComponent> TextInput::Clone()
{
    auto clone = utils::make_unique<TextInput>(GetUIContext(), m_width, m_height);
    clone->m_inputText = m_inputText;
    return clone;
}

void TextInput::Render(RendererT& o_renderStream) const
{
    m_renderStyle->Render(o_renderStream);
    o_renderStream << utils::Format("\033[{}B", m_height); // Move cursor to the bottom of the rendered component
}

void TextInput::SetWidth(const uint16_t& i_width)
{
    m_width = i_width;
}

void TextInput::SetHeight(const uint16_t& i_height)
{
    m_height = i_height;
}

uint16_t TextInput::GetWidth() const
{
    return m_width;
}

uint16_t TextInput::GetHeight() const
{
    return m_height;
}

void TextInput::SetText(const std::string& i_text)
{
    m_inputText = i_text;
}

const std::string& TextInput::GetText() const
{
    return m_inputText;
}

void TextInput::SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle)
{
    i_renderStyle->BindAttribute("width", utils::attribute::make_bind_attribute(m_width));
    i_renderStyle->BindAttribute("height", utils::attribute::make_bind_attribute(m_height));
    i_renderStyle->BindAttribute("text", utils::attribute::make_bind_attribute(m_inputText));
    IUIComponent::SetRenderStyle(std::move(i_renderStyle));
}

void TextInput::OnFocusGained(const utils::RGBColor& i_focusColor)
{
    FocusUIComponent(*this, i_focusColor);
}

void TextInput::OnFocusLost()
{
    UnFocusUIComponent(*this);
}

bool TextInput::ProcessInput(const std::string& i_input) const
{
    TextInput& _this = const_cast<TextInput&>(*this);
    if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::BackSpace))
    {
        if (!m_inputText.empty())
        {
            _this.m_inputText.pop_back();
        }
    }
    else if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::Enter))
    {
        utils::Access<ITextInput::SignalKey>(_this.sig_onTextChanged).Emit(_this.m_inputText);
    }
    else
    {
        if (std::optional<bool> filterResult = utils::Access<ITextInput::SignalKey>(_this.sig_onTextFiltered).Emit(i_input); !filterResult.has_value() || filterResult.value())
        {
            _this.m_inputText.append(i_input);
        }
    }
    return true;
}

void TextInput::InitializeInputHints(IInputHints& i_inputHints) const
{
    i_inputHints.AddHint(ActionCode::Enter, "Confirm Input");
    i_inputHints.AddHint(ActionCode::BackSpace, "Delete Last Character");
}