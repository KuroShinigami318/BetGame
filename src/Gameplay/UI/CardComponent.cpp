#include "stdafx.h"
#include "Gameplay/UI/CardComponent.h"
#include "UI/UIHelper.h"
#include "attribute.h"

CardComponent::CardComponent(const UIContext& i_uiContext, const uint16_t& i_id, const uint16_t& i_width, const uint16_t& i_height)
	: IUIComponent(i_uiContext)
	, m_id(i_id)
	, m_width(i_width)
	, m_height(i_height)
	, m_multiplier(1)
	, m_bid(0)
{
}

void CardComponent::SetMultiplier(const uint16_t& i_multiplier)
{
	m_multiplier = i_multiplier;
}

const uint16_t& CardComponent::GetMultiplierValue() const
{
	return m_multiplier;
}

void CardComponent::Bid(const uint32_t& i_bidVal)
{
	m_bid += i_bidVal;
}

void CardComponent::ResetBid(const uint32_t& i_bidVal)
{
	m_bid = i_bidVal;
}

const uint32_t& CardComponent::GetBid() const
{
	return m_bid;
}

std::optional<uint32_t> CardComponent::Roll()
{
	if (operator()())
	{
		return m_bid * m_multiplier;
	}
	return std::nullopt;
}

void CardComponent::SetWidth(const uint16_t& i_width)
{
	m_width = i_width;
}

void CardComponent::SetHeight(const uint16_t& i_height)
{
	m_height = i_height;
}

uint16_t CardComponent::GetWidth() const
{
	return m_width;
}

uint16_t CardComponent::GetHeight() const
{
	return m_height;
}

void CardComponent::SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle)
{
	static struct title{} _title;
	static struct body{} _body;

	i_renderStyle->BindAttribute("title", utils::attribute::make_value_attribute(_title, {[this](const title&) { return utils::Format("Card #{} - x{}", m_id, m_multiplier); }}));
	i_renderStyle->BindAttribute("body", utils::attribute::make_value_attribute(_body, {[this](const body&) { return utils::Format("Bid: {}", m_bid); }}));
	i_renderStyle->BindAttribute("width", utils::attribute::make_bind_attribute(m_width));
	i_renderStyle->BindAttribute("height", utils::attribute::make_bind_attribute(m_height));
	IUIComponent::SetRenderStyle(std::move(i_renderStyle));
}

void CardComponent::Render(RendererT& o_renderStream) const
{
	if (m_renderStyle)
	{
		m_renderStyle->Render(o_renderStream);
	}
}

void CardComponent::OnFocusGained(const utils::RGBColor& i_focusColor)
{
	FocusUIComponent(*this, i_focusColor);
}

void CardComponent::OnFocusLost()
{
	UnFocusUIComponent(*this);
}

utils::unique_ref<IComponent> CardComponent::Clone()
{
	utils::unique_ref<CardComponent> clone = utils::make_unique<CardComponent>(GetUIContext(), m_id, m_width, m_height);
	clone->SetMultiplier(m_multiplier);
	clone->Bid(m_bid);
	clone->SetRate(m_probability.GetRate());
	return clone;
}

void CardComponent::SetRate(const float& i_rate)
{
	m_probability.SetRate(i_rate);
}

const float& CardComponent::GetRate() const
{
	return m_probability.GetRate();
}

bool CardComponent::operator()() const
{
	return m_probability();
}