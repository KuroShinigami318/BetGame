#include "stdafx.h"
#include "Components/InputSelection.h"
#include "Components/IOption.h"
#include "Control/ActionCode.h"
#include "UI/IInputHints.h"
#include "UI/LabelRenderStyle.h"
#include "UI/UIHelper.h"
#include "UI/IUIManager.h"

#include "attribute.h"
#include "intrusive_list.h"

namespace
{
class NullOption : public IOption
{
public:
    const std::string& GetText() const override
    {
        static const std::string emptyString;
        return emptyString;
    }
};
}

InputSelection::~InputSelection()
{
    ClearOptions();
}

InputSelection::InputSelection(const UIContext& i_uiContext, const uint16_t& i_width, const uint16_t& i_height)
    : IUIComponent(i_uiContext)
    , m_width(i_width)
    , m_height(i_height)
    , m_listEndOption(utils::make_unique<NullOption>())
    , m_optionsList(utils::make_unique<utils::intrusive_list>())
    , m_selectedOption(m_listEndOption.get())
{
    m_optionsList->end(m_listEndOption.get());
    SetRenderStyle(utils::make_unique<LabelRenderStyle>());
}

void InputSelection::SetWidth(const uint16_t& i_width)
{
    m_width = i_width;
}

void InputSelection::SetHeight(const uint16_t& i_height)
{
    m_height = i_height;
}

uint16_t InputSelection::GetWidth() const
{
    return m_width;
}

uint16_t InputSelection::GetHeight() const
{
    return m_height;
}

void InputSelection::SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle)
{
    static const struct Text {} _text;
    i_renderStyle->BindAttribute("width", utils::attribute::make_bind_attribute(m_width));
    i_renderStyle->BindAttribute("height", utils::attribute::make_bind_attribute(m_height));
    i_renderStyle->BindAttribute("text", utils::attribute::make_bind_attribute(_text, {[this](const Text&) { return utils::Format("< {} >", m_selectedOption->GetText()); }}));
    IUIComponent::SetRenderStyle(std::move(i_renderStyle));
}

void InputSelection::Render(RendererT& o_renderStream) const
{
    m_renderStyle->Render(o_renderStream);
    o_renderStream << utils::Format("\033[{}B", m_height); // Move cursor to the bottom of the rendered component
}

void InputSelection::OnFocusGained(const utils::RGBColor& i_focusColor)
{
    FocusUIComponent(*this, i_focusColor);
}

void InputSelection::OnFocusLost()
{
    UnFocusUIComponent(*this);
}

bool InputSelection::ProcessInput(const std::string& i_input) const
{
    InputSelection* self = const_cast<InputSelection*>(this);
    if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::LeftArrow))
    {
        self->SwitchToPreviousOption();
        return true;
    }
    if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::RightArrow))
    {
        self->SwitchToNextOption();
        return true;
    }

    return false;
}

void InputSelection::InitializeInputHints(IInputHints& i_inputHints) const
{
    i_inputHints.AddHint(ActionCode::LeftArrow, "Previous Option");
    i_inputHints.AddHint(ActionCode::RightArrow, "Next Option");
}

void InputSelection::AddOptions(std::initializer_list<utils::unique_ref<IOption>> i_options)
{
    for (auto& option : i_options)
    {
        AddOption(std::move(const_cast<utils::unique_ref<IOption>&>(option)));
    }
}

void InputSelection::AddOption(utils::unique_ref<IOption> i_option)
{
    if (m_selectedOption == m_listEndOption.get())
    {
        m_selectedOption = i_option.get();
        utils::Access<SignalKey>(sig_onOptionChanged).Emit(*m_selectedOption);
    }
    utils::unique_ptr<IOption> rawOptionPtr = std::move(i_option);
    m_optionsList->push_back(*rawOptionPtr.release());
}

void InputSelection::RemoveOption(IOption& i_option)
{
    ASSERT(i_option.next && i_option.prev, "Option is expected to be part of the options list.");
    if (i_option.next == nullptr || i_option.prev == nullptr)
    {
        return;
    }
    if (m_selectedOption == &i_option)
    {
        m_selectedOption = static_cast<IOption*>(i_option.next);
    }
    delete &m_optionsList->remove(i_option);
}

void InputSelection::ClearOptions()
{
    m_optionsList->deallocate_all();
    m_selectedOption = m_listEndOption.get();
}

void InputSelection::SetAsSelectedOption(IOption& i_option)
{
    ASSERT(i_option.next && i_option.prev, "Option is expected to be part of the options list.");
    if (i_option.next == nullptr || i_option.prev == nullptr || m_selectedOption == &i_option || &i_option == m_listEndOption.get())
    {
        return;
    }
    m_selectedOption = &i_option;
    utils::Access<SignalKey>(sig_onOptionChanged).Emit(i_option);
}

IOption* InputSelection::GetSelectedOption() const
{
    return m_selectedOption != m_listEndOption.get() ? m_selectedOption : nullptr;
}

void InputSelection::SwitchToNextOption()
{
    do
    {
        m_selectedOption = static_cast<IOption*>(m_selectedOption->next);
    } while (m_selectedOption == m_listEndOption.get() && !m_optionsList->empty());
    if (m_selectedOption != m_listEndOption.get())
    {
        utils::Access<SignalKey>(sig_onOptionChanged).Emit(*m_selectedOption);
    }
}

void InputSelection::SwitchToPreviousOption()
{
    do
    {
        m_selectedOption = static_cast<IOption*>(m_selectedOption->prev);
    } while (m_selectedOption == m_listEndOption.get() && !m_optionsList->empty());
    utils::Access<SignalKey>(sig_onOptionChanged).Emit(*m_selectedOption);
    if (m_selectedOption != m_listEndOption.get())
    {
        utils::Access<SignalKey>(sig_onOptionChanged).Emit(*m_selectedOption);
    }
}

utils::unique_ref<IComponent> InputSelection::Clone()
{
    return utils::make_unique<InputSelection>(GetUIContext(), GetWidth(), GetHeight());
}