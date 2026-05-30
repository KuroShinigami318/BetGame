#include "stdafx.h"
#include "Components/HorizontalContainer.h"

HorizontalContainer::HorizontalContainer(const UIContext& i_uiContext, const uint16_t& i_width)
    : IUIComponent(i_uiContext)
    , SwitchContainer(i_uiContext)
    , m_width(i_width)
{
}

HorizontalContainer::HorizontalContainer(const HorizontalContainer& other)
    : IUIComponent(other.GetUIContext())
    , SwitchContainer(other.GetUIContext())
{
    *this = other;
}

HorizontalContainer::HorizontalContainer(HorizontalContainer&& other) noexcept
    : IUIComponent(std::move(other))
    , SwitchContainer(std::move(other))
    , m_width(std::exchange(other.m_width, 0))
    , m_height(std::exchange(other.m_height, 0))
{
}

HorizontalContainer& HorizontalContainer::operator=(const HorizontalContainer& other)
{
    if (this != &other)
    {
        SwitchContainer::operator=(other);
        m_width = other.m_width;
        m_height = other.m_height;
    }
    return *this;
}

HorizontalContainer& HorizontalContainer::operator=(HorizontalContainer&& other) noexcept
{
    if (this != &other)
    {
        SwitchContainer::operator=(std::move(other));
        m_width = std::exchange(other.m_width, 0);
        m_height = std::exchange(other.m_height, 0);
    }
    return *this;
}

void HorizontalContainer::SetWidth(const uint16_t& i_width)
{
    m_width = i_width;
    uint16_t componentWidth = m_width / GetComponentCount();
    ComponentListTag* listTag;
    GetBegin(listTag);
    while (!IsEnd(listTag))
    {
        GetUIComponent(listTag)->SetWidth(componentWidth);
        Next(listTag);
    }
}

uint16_t HorizontalContainer::GetWidth() const
{
    if (m_width != 0)
    {
        return m_width;
    }

    uint16_t totalWidth = 0;
    ComponentListTag* listTag;
    GetBegin(listTag);
    while (!IsEnd(listTag))
    {
        totalWidth += GetUIComponent(listTag)->GetWidth();
        Next(listTag);
    }
    return const_cast<uint16_t&>(m_width) = totalWidth;
}

uint16_t HorizontalContainer::GetHeight() const
{
    if (m_height != 0)
    {
        return m_height;
    }

    uint16_t maxHeight = 0;
    ComponentListTag* listTag;
    GetBegin(listTag);
    while (!IsEnd(listTag))
    {
        const uint16_t componentHeight = GetUIComponent(listTag)->GetHeight();
        maxHeight = (std::max)(maxHeight, componentHeight);
        Next(listTag);
    }
    return const_cast<uint16_t&>(m_height) = maxHeight;
}

void HorizontalContainer::Render(RendererT& o_renderStream) const
{
    size_t totalWidth = 0;
    size_t maxHeight = 0;
    ComponentListTag* listTag;
    GetBegin(listTag);
    while (!IsEnd(listTag))
    {
        IUIComponent& uiComponent = *GetUIComponent(listTag);
        uiComponent.Render(o_renderStream);
        const uint16_t componentWidth = uiComponent.GetWidth();
        const uint16_t componentHeight = uiComponent.GetHeight();
        o_renderStream << utils::Format("\033[{}A", componentHeight); // Move cursor to the top of the rendered component
        o_renderStream << utils::Format("\033[{}C", componentWidth); // Move cursor to the right of the rendered component
        totalWidth += componentWidth;
        maxHeight = (std::max)(maxHeight, static_cast<const size_t&>(componentHeight));
        Next(listTag);
    }
    o_renderStream << utils::Format("\033[{}B", maxHeight); // Move cursor to the bottom of the rendered components
    o_renderStream << utils::Format("\033[{}D", totalWidth); // Move cursor to the left of the rendered components
}

utils::unique_ref<IComponent> HorizontalContainer::Clone()
{
    return utils::make_unique<HorizontalContainer>(*this);
}