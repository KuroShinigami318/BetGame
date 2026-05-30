#pragma once
#include "HorizontalContainer.h"

class ILabel;

class SettingComponent : public HorizontalContainer, utils::noncopy, utils::nonmove
{
public:
    SettingComponent(const UIContext& i_uiContext);
    void SetWidth(const uint16_t& i_width) override;
    void SetHeight(const uint16_t& i_height) override;
    uint16_t GetWidth() const override;
    uint16_t GetHeight() const override;
    void Render(RendererT& o_renderStream) const override;
    void OnFocusGained(const utils::RGBColor& i_focusColor) override;
    void OnFocusLost() override;
    utils::unique_ref<IComponent> Clone() override;

protected:
    template <typename InputOptionT, typename IInputOptionT, typename LabelComponentT>
    void Initialize(const std::string& i_text, const uint16_t& i_width, const uint16_t& i_height, ILabel*& o_label, IInputOptionT*& o_inputOption)
    {
        uint16_t labelWidth = i_width / 3;
        MakeAndMapUIComponent<LabelComponentT>(o_label, i_text, labelWidth, i_height);
        MakeAndMapUIComponent<InputOptionT>(o_inputOption, i_width - labelWidth, i_height);
    }
};