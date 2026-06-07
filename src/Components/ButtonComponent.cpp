#include "stdafx.h"
#include "Components/ButtonComponent.h"
#include "Control/ActionCode.h"
#include "UI/IInputHints.h"
#include "UI/IUIManager.h"
#include "UI/UIHelper.h"

ButtonComponent::ButtonComponent(const UIContext& i_uiContext, const std::string& i_text, const uint16_t& i_width, const uint16_t& i_height)
    : IUIComponent(i_uiContext)
    , LabelComponent(i_uiContext, i_text, i_width, i_height)
{
}

bool ButtonComponent::ProcessInput(const std::string& i_input) const
{
    if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::Enter))
    {
        ButtonComponent& self = const_cast<ButtonComponent&>(*this);
        utils::Access<IButton::SignalKey>(self.sig_onAction).Emit();
        return true;
    }
    return false;
}

void ButtonComponent::InitializeInputHints(IInputHints& i_inputHints) const
{
    i_inputHints.AddHint(ActionCode::Enter, "Activate Button");
}

void ButtonComponent::OnFocusGained(const utils::RGBColor& i_focusColor)
{
    FocusUIComponent(*this, i_focusColor);
}

void ButtonComponent::OnFocusLost()
{
    UnFocusUIComponent(*this);
}

utils::unique_ref<IComponent> ButtonComponent::Clone()
{
    return utils::make_unique<ButtonComponent>(GetUIContext(), GetText(), GetWidth(), GetHeight());
}