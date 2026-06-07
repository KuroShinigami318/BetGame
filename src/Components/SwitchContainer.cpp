#include "stdafx.h"
#include "Components/SwitchContainer.h"
#include "UI/IInputHints.h"
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

void SwitchContainer::OnFocusGained(const utils::RGBColor& i_focusColor)
{
    if (!IsEnd(m_currentComponentTag))
    {
        GetUIComponent(m_currentComponentTag)->OnFocusGained(i_focusColor);
    }
}

void SwitchContainer::OnFocusLost()
{
    if (!IsEnd(m_currentComponentTag))
    {
        GetUIComponent(m_currentComponentTag)->OnFocusLost();
    }
}

bool SwitchContainer::ProcessInput(const std::string& i_input) const
{
    if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::SwitchNext) || m_uiContext.uiManager.IsInputAction(i_input, ActionCode::SwitchPrev))
    {
        bool switched = false;
        InteractiveComponentTag* oldComponentTag = m_currentComponentTag;
        SwitchContainer* self = const_cast<SwitchContainer*>(this);
        if (SwitchContainer* switchContainer = dynamic_cast<SwitchContainer*>(GetInputRelay(m_currentComponentTag)))
        {
            utils::Connection onActiveComponentChangedConnection = switchContainer->sig_onActiveComponentChanged.Connect(&SwitchContainer::OnActiveComponentChanged, self);
            switched = switchContainer->ProcessInput(i_input);
        }
        if (!switched)
        {
            if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::SwitchNext))
            {
                self->SwitchToNextInputComponent();
            }
            else if (m_uiContext.uiManager.IsInputAction(i_input, ActionCode::SwitchPrev))
            {
                self->SwitchToPrevInputComponent();
            }
            if (switched = (m_currentComponentTag != oldComponentTag))
            {
                self->OnActiveComponentChanged(*GetUIComponent(m_currentComponentTag));
            }
        }
        return switched;
    }

    if (!IsEnd(m_currentComponentTag))
    {
        return GetInputRelay(m_currentComponentTag)->ProcessInput(i_input);
    }

    return false;
}

void SwitchContainer::InitializeInputHints(IInputHints& i_inputHints) const
{
    i_inputHints.AddHint(ActionCode::SwitchNext, "Switch Next Focus");
    i_inputHints.AddHint(ActionCode::SwitchPrev, "Switch Previous Focus");
    if (!IsEnd(m_currentComponentTag))
    {
        return GetInputRelay(m_currentComponentTag)->InitializeInputHints(i_inputHints);
    }
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

bool SwitchContainer::SwitchToPrevInputComponent(const utils::RGBColor& i_focusColor)
{
    InteractiveComponentTag* firstInteractiveComponentTag;
    GetFirstInteractive(firstInteractiveComponentTag);
    if (!IsEnd(m_currentComponentTag))
    {
        GetUIComponent(m_currentComponentTag)->OnFocusLost();
    }
    do
    {
        Prev(const_cast<InteractiveComponentTag*&>(m_currentComponentTag));
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
            OnActiveComponentChanged(*GetUIComponent(m_currentComponentTag));
            return true;
        }
        Next(listTag);
    }
    return false;
}

void SwitchContainer::OnActiveComponentChanged(const IUIComponent& i_activeComponent)
{
    utils::Access<SignalKey>(sig_onActiveComponentChanged).Emit(i_activeComponent);
}