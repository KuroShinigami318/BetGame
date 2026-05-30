#pragma once

#include "ContainerBase.h"

class SwitchContainer : public ContainerBase, virtual public IInputRelay
{
public:
    using ContainerBase::ContainerBase;
    using ContainerBase::operator=;
    SwitchContainer(const UIContext& i_uiContext);
    SwitchContainer(const SwitchContainer& other);
    SwitchContainer(SwitchContainer&& other) noexcept;
    SwitchContainer& operator=(const SwitchContainer& other);
    SwitchContainer& operator=(SwitchContainer&& other) noexcept;
    utils::unique_ref<IComponent> Clone() override;
    bool ProcessInput(const std::string&) const override;
    bool SwitchToNextInputComponent(const utils::RGBColor& i_focusColor = utils::RGBColor(0, 255, 0));
    bool SetActiveComponent(IUIComponent& i_uiComponent, const utils::RGBColor& i_focusColor = utils::RGBColor(0, 255, 0));

private:
    InteractiveComponentTag* m_currentComponentTag;
};