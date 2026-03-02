#include "stdafx.h"
#include "Gameplay/UI/CardRenderStyle.h"
#include "UI/LabelRenderStyle.h"
#include "attribute.h"

CardRenderStyle::CardRenderStyle()
    : m_titleRenderStyle(utils::make_unique<LabelRenderStyle>())
    , m_bodyRenderStyle(utils::make_unique<LabelRenderStyle>())
{
}

CardRenderStyle::CardRenderStyle(const CardRenderStyle& other)
    : AttributesMap(other)
	, m_titleRenderStyle(other.m_titleRenderStyle->Clone())
	, m_bodyRenderStyle(other.m_bodyRenderStyle->Clone())
{
}

CardRenderStyle& CardRenderStyle::operator=(const CardRenderStyle& other)
{
	if (this != &other)
	{
		static_cast<AttributesMap&>(*this).operator=(other);
		m_titleRenderStyle = other.m_titleRenderStyle->Clone();
		m_bodyRenderStyle = other.m_bodyRenderStyle->Clone();
	}

	return *this;
}

void CardRenderStyle::OnThisRenderStyleApplied() const
{
	static utils::attribute widthAttr = utils::attribute::make_attribute<uint16_t>(0);
	static utils::attribute heightAttr = utils::attribute::make_attribute<uint16_t>(0);
	static utils::attribute textAttr = utils::attribute::make_attribute<std::string>();
	const uint16_t* cardHeight = nullptr;
	GetAttributeOrDefault("height", heightAttr).get(cardHeight).assertSuccess();
	uint16_t titleHeight = *cardHeight / 2;
	uint16_t bodyHeight = *cardHeight - titleHeight;
    m_titleRenderStyle->BindAttribute("width", GetAttributeOrDefault("width", widthAttr));
    m_titleRenderStyle->BindAttribute("height", utils::attribute::make_value_attribute(titleHeight));
    m_titleRenderStyle->BindAttribute("text", GetAttributeOrDefault("title", textAttr));
	m_bodyRenderStyle->BindAttribute("width", GetAttributeOrDefault("width", widthAttr));
	m_bodyRenderStyle->BindAttribute("height", utils::attribute::make_value_attribute(bodyHeight));
	m_bodyRenderStyle->BindAttribute("text", GetAttributeOrDefault("body", textAttr));
}

void CardRenderStyle::Render(RendererT& o_renderStream) const
{
	static utils::attribute isFocusedAttr = utils::attribute::make_attribute<bool>(false);
	const bool* isFocused = nullptr;
	GetAttributeOrDefault("isFocused", isFocusedAttr).get<bool>(isFocused).ignoreResult();
	if (*isFocused)
	{
		m_titleRenderStyle->BindAttribute("text_format", utils::attribute::make_attribute<utils::Log::TextFormat>(utils::Log::TextStyle::Bold, utils::RGBColor(255, 255, 0)));
		m_bodyRenderStyle->BindAttribute("text_format", utils::attribute::make_attribute<utils::Log::TextFormat>(utils::Log::TextStyle::Bold, utils::RGBColor(255, 255, 0)));
	}
	else
	{
		m_titleRenderStyle->UnbindAttribute("text_format");
		m_bodyRenderStyle->UnbindAttribute("text_format");
	}
	m_titleRenderStyle->Render(o_renderStream);
	const uint16_t* titleHeight = nullptr;
	m_titleRenderStyle->GetAttribute("height")->get(titleHeight).ignoreResult();
    o_renderStream << utils::Format("\033[{}B", *titleHeight);
	m_bodyRenderStyle->Render(o_renderStream);
	o_renderStream << utils::Format("\033[{}A", *titleHeight);
}

utils::unique_ref<IRenderStyle> CardRenderStyle::Clone() const
{
    return utils::make_unique<CardRenderStyle>(*this);
}
