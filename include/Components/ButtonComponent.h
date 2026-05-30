#pragma once
#include "Control/IInputRelay.h"
#include "IButton.h"
#include "LabelComponent.h"

class ButtonComponent : virtual public IButton, public LabelComponent, public IInputRelay
{
public:
    ButtonComponent(const UIContext& i_uiContext, const std::string& i_text = "", const uint16_t& i_width = 0, const uint16_t& i_height = 0);
    bool ProcessInput(const std::string& i_input) const override;
    void OnFocusGained(const utils::RGBColor& i_focusColor) override;
    void OnFocusLost() override;
    utils::unique_ref<IComponent> Clone() override;
};