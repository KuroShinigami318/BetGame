#include "stdafx.h"
#include "Components/SwitchContainer.h"
#include "UI/IUIManager.h"
#include "Control/ActionCode.h"

SwitchContainer::SwitchContainer(const UIContext& i_uiContext)
    : IUIComponent(i_uiContext)
    , ContainerBase(i_uiContext)
    , m_currentComponentTag(nullptr)
{
    GetFirstInteractive(m_currentComponentTag);
}

SwitchContainer::SwitchContainer(const SwitchContainer& other)
    : IUIComponent(other.GetUIContext())
    , ContainerBase(other.GetUIContext())
{
    *this = other;
}

SwitchContainer::SwitchContainer(SwitchContainer&& other) noexcept
    : IUIComponent(std::move(other))
    , ContainerBase(std::move(other))
    , m_currentComponentTag(nullptr)
{
    GetFirstInteractive(m_currentComponentTag);
}

SwitchContainer& SwitchContainer::operator=(const SwitchContainer& other)
{
    if (this != &other)
    {
        ContainerBase::operator=(other);
        GetFirstInteractive(m_currentComponentTag);
    }
    return *this;
}

SwitchContainer& SwitchContainer::operator=(SwitchContainer&& other) noexcept
{
    if (this != &other)
    {
        ContainerBase::operator=(std::move(other));
        GetFirstInteractive(m_currentComponentTag);
    }
    return *this;
}

utils::unique_ref<IComponent> SwitchContainer::Clone()
{
    return utils::make_unique<SwitchContainer>(*this);
}

bool SwitchContainer::ProcessInput(const std::string& i_input) const
{
    if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::Switch))
    {
        bool switched = false;
        if (SwitchContainer* switchContainer = dynamic_cast<SwitchContainer*>(GetInputRelay(m_currentComponentTag)))
        {
            switched = switchContainer->ProcessInput(i_input);
        }
        return switched || const_cast<SwitchContainer*>(this)->SwitchToNextInputComponent();
    }

    if (!IsEnd(m_currentComponentTag))
    {
        return GetInputRelay(m_currentComponentTag)->ProcessInput(i_input);
    }

    return false;
}

bool SwitchContainer::SwitchToNextInputComponent(const utils::RGBColor& i_focusColor)
{
    InteractiveComponentTag* firstInteractiveComponentTag;
    GetFirstInteractive(firstInteractiveComponentTag);
    if (!IsEnd(m_currentComponentTag))
    {
        GetUIComponent(m_currentComponentTag)->OnFocusLost();
    }
    do
    {
        Next(const_cast<InteractiveComponentTag*&>(m_currentComponentTag));
    } while (m_currentComponentTag != firstInteractiveComponentTag && !GetInputRelay(m_currentComponentTag));
    if (!IsEnd(m_currentComponentTag))
    {
        GetUIComponent(m_currentComponentTag)->OnFocusGained(i_focusColor);
    }
    return m_currentComponentTag != firstInteractiveComponentTag;
}

bool SwitchContainer::SetActiveComponent(IUIComponent& i_uiComponent, const utils::RGBColor& i_focusColor)
{
    if (dynamic_cast<IInputRelay*>(&i_uiComponent) == nullptr || GetUIComponent(m_currentComponentTag) == &i_uiComponent)
    {
        return false;
    }
    InteractiveComponentTag* listTag;
    GetFirstInteractive(listTag);
    while (!IsEnd(listTag))
    {
        if (GetUIComponent(listTag) == &i_uiComponent)
        {
            if (!IsEnd(m_currentComponentTag))
            {
                GetUIComponent(m_currentComponentTag)->OnFocusLost();
            }
            m_currentComponentTag = listTag;
            GetUIComponent(m_currentComponentTag)->OnFocusGained(i_focusColor);
            return true;
        }
        Next(listTag);
    }
    return false;
}