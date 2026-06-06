#pragma once

#include "ContainerBase.h"

class SwitchContainer : public ContainerBase, virtual public IInputRelay
{
private:
    struct SignalKey;

public:
    using ContainerBase::ContainerBase;
    using ContainerBase::operator=;
    SwitchContainer(const UIContext& i_uiContext);
    SwitchContainer(const SwitchContainer& other);
    SwitchContainer(SwitchContainer&& other) noexcept;
    SwitchContainer& operator=(const SwitchContainer& other);
    SwitchContainer& operator=(SwitchContainer&& other) noexcept;
    utils::unique_ref<IComponent> Clone() override;
    void OnFocusGained(const utils::RGBColor& i_focusColor) override;
    void OnFocusLost() override;
    bool ProcessInput(const std::string&) const override;
    void InitializeInputHints(IInputHints& i_inputHints) const override;
    bool SwitchToNextInputComponent(const utils::RGBColor& i_focusColor = utils::RGBColor(0, 255, 0));
    bool SwitchToPrevInputComponent(const utils::RGBColor& i_focusColor = utils::RGBColor(0, 255, 0));
    bool SetActiveComponent(IUIComponent& i_uiComponent, const utils::RGBColor& i_focusColor = utils::RGBColor(0, 255, 0));

private:
    void OnActiveComponentChanged(const IUIComponent& i_activeComponent);

public:
    utils::Signal_public<void(const IUIComponent&), SignalKey> sig_onActiveComponentChanged;

private:
    InteractiveComponentTag* m_currentComponentTag;
};