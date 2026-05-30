#pragma once
#include "Components/SwitchContainer.h"

class HorizontalContainer : public SwitchContainer
{
public:
    using SwitchContainer::SwitchContainer;
    using SwitchContainer::operator=;
    HorizontalContainer(const UIContext& i_uiContext, const uint16_t& i_width = 0);
    HorizontalContainer(const HorizontalContainer& other);
    HorizontalContainer(HorizontalContainer&& other) noexcept;
    HorizontalContainer& operator=(const HorizontalContainer& other);
    HorizontalContainer& operator=(HorizontalContainer&& other) noexcept;
    void SetWidth(const uint16_t& i_width);
    uint16_t GetWidth() const override;
    uint16_t GetHeight() const override;
    void Render(RendererT& o_renderStream) const override;
    utils::unique_ref<IComponent> Clone() override;

private:
    uint16_t m_width = 0;
    uint16_t m_height = 0;
};