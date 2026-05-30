#pragma once

#include "Gameplay/Logic/ICard.h"
#include "UI/IUIComponent.h"
#include "Components/Probability.h"

class CardComponent : public ICard, public IUIComponent
{
public:
	CardComponent(const UIContext& i_uiContext, const uint16_t& i_id, const uint16_t& i_width, const uint16_t& i_height);
	void SetMultiplier(const uint16_t&) override;
	const uint16_t& GetMultiplierValue() const override;
	void Bid(const uint32_t&) override;
	void ResetBid(const uint32_t&) override;
	const uint32_t& GetBid() const override;
	std::optional<uint32_t> Roll() override;
	void SetWidth(const uint16_t&) override;
	void SetHeight(const uint16_t&) override;
	uint16_t GetWidth() const override;
	uint16_t GetHeight() const override;
	void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) override;
	void Render(RendererT& o_renderStream) const override;
	void OnFocusGained(const utils::RGBColor& i_focusColor) override;
	void OnFocusLost() override;
	utils::unique_ref<IComponent> Clone() override;
	void SetRate(const float& i_rate) override;
	const float& GetRate() const override;
	bool operator()() const override;

private:
	uint16_t m_id;
	uint16_t m_width;
	uint16_t m_height;
	uint16_t m_multiplier;
	uint32_t m_bid;
	Probability m_probability;
};