#include "stdafx.h"
#include "Gameplay/UI/BidRenderStyle.h"
#include "UI/LabelRenderStyle.h"
#include "attribute.h"

BidRenderStyle::BidRenderStyle()
	: m_labelRenderStyle(utils::make_unique<LabelRenderStyle>())
{
}

BidRenderStyle::BidRenderStyle(const BidRenderStyle& other)
	: AttributesMap(other)
	, m_labelRenderStyle(other.m_labelRenderStyle->Clone())
{
}

BidRenderStyle& BidRenderStyle::operator=(const BidRenderStyle& other)
{
	if (this != &other)
	{
		static_cast<AttributesMap&>(*this).operator=(other);
		m_labelRenderStyle = other.m_labelRenderStyle->Clone();
	}
	return *this;
}

void BidRenderStyle::OnThisRenderStyleApplied() const
{
	static utils::attribute textAttr = utils::attribute::make_attribute<std::string>();
	static utils::attribute widthAttr = utils::attribute::make_attribute<uint16_t>(0);
	static utils::attribute heightAttr = utils::attribute::make_attribute<uint16_t>(0);
	m_labelRenderStyle->BindAttribute("text", GetAttributeOrDefault("text", textAttr));
	m_labelRenderStyle->BindAttribute("width", GetAttributeOrDefault("width", widthAttr));
	m_labelRenderStyle->BindAttribute("height", GetAttributeOrDefault("height", heightAttr));
}

void BidRenderStyle::Render(RendererT& o_renderStream) const
{
	m_labelRenderStyle->Render(o_renderStream);
}

utils::unique_ref<IRenderStyle> BidRenderStyle::Clone() const
{
    return utils::make_unique<BidRenderStyle>(*this);
}
