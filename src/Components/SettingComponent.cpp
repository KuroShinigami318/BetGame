#include "stdafx.h"
#include "Components/SettingComponent.h"

SettingComponent::SettingComponent(const UIContext& i_uiContext)
    : IUIComponent(i_uiContext)
    , HorizontalContainer(i_uiContext)
{
}

void SettingComponent::SetWidth(const uint16_t& i_width)
{
    uint16_t labelWidth = i_width / 3;
    ComponentListTag* listTag;
    GetBegin(listTag);
    IUIComponent* labelComponent = GetUIComponent(listTag);
    ASSERT(labelComponent, "Label component is expected to be the first component in the list.");
    labelComponent->SetWidth(labelWidth);
    Next(listTag);
    IUIComponent* settingComponent = GetUIComponent(listTag);
    ASSERT(settingComponent, "Setting component is expected to be the second component in the list.");
    settingComponent->SetWidth(i_width - labelWidth);
}

void SettingComponent::SetHeight(const uint16_t& i_height)
{
    ComponentListTag* listTag;
    GetBegin(listTag);
    while (!IsEnd(listTag))
    {
        IUIComponent* component = GetUIComponent(listTag);
        ASSERT(component, "Component is expected in the list.");
        component->SetHeight(i_height);
        Next(listTag);
    }
}

uint16_t SettingComponent::GetWidth() const
{
    return HorizontalContainer::GetWidth();
}

uint16_t SettingComponent::GetHeight() const
{
    return HorizontalContainer::GetHeight();
}

void SettingComponent::Render(RendererT& o_renderStream) const
{
    HorizontalContainer::Render(o_renderStream);
}

void SettingComponent::OnFocusGained(const utils::RGBColor& i_focusColor)
{
    ComponentListTag* listTag;
    GetBegin(listTag);
    while (!IsEnd(listTag))
    {
        IUIComponent* component = GetUIComponent(listTag);
        ASSERT(component, "Component is expected in the list.");
        component->OnFocusGained(i_focusColor);
        Next(listTag);
    }
    SwitchToNextInputComponent(i_focusColor);
}

void SettingComponent::OnFocusLost()
{
    ComponentListTag* listTag;
    GetBegin(listTag);
    while (!IsEnd(listTag))
    {
        IUIComponent* component = GetUIComponent(listTag);
        ASSERT(component, "Component is expected in the list.");
        component->OnFocusLost();
        Next(listTag);
    }
}

utils::unique_ref<IComponent> SettingComponent::Clone()
{
    return utils::make_unique<SettingComponent>(GetUIContext());
}