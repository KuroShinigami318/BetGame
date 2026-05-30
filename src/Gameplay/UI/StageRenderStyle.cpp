#include "stdafx.h"
#include "Gameplay/UI/StageRenderStyle.h"
#include "UI/IUIComponent.h"
#include "AttributeHelper.h"

StageRenderStyle::StageRenderStyle(utils::unique_ref<IRenderStyle> i_cardRenderStyle)
	: m_cardRenderStyle(std::move(i_cardRenderStyle))
{
}

void StageRenderStyle::Render(RendererT& o_renderStream) const
{
	const CardComponetsT* cardComponents = nullptr;
	const uint16_t* width = nullptr;
	const uint16_t* height = nullptr;
	utils::attribute* cardComponentsAttr = GetAttribute("cardComponents");
	utils::attribute* widthAttr = GetAttribute("width");
	utils::attribute* heightAttr = GetAttribute("height");
	if (!cardComponentsAttr || !widthAttr || !heightAttr)
	{
		return;
	}
	std::optional<utils::attribute::result_t> getResult;
	GetAttributeValue(getResult, *cardComponentsAttr, cardComponents);
	GetAttributeValue(getResult, *widthAttr, width);
	GetAttributeValue(getResult, *heightAttr, height);
	if (getResult->isErr() || cardComponents->empty() || cardComponents->at(0)->GetWidth() == 0)
	{
		return;
	}
	const size_t itemsPerRow = (*width) / cardComponents->at(0)->GetWidth();
	const double rowCount = (double)cardComponents->size() / itemsPerRow;
	ASSERT_PLAIN_MSG(rowCount < *height, "Not enough height to render all cards. expected minimum height:{}, but actual height:{}", rowCount, *height);
	for (size_t row = 0; row < rowCount; ++row)
	{
		for (size_t col = 0; col < itemsPerRow; ++col)
		{
			size_t cardIndex = row * itemsPerRow + col;
			if (cardIndex >= cardComponents->size())
			{
				break;
			}
			cardComponents->at(cardIndex)->Render(o_renderStream);
			o_renderStream << utils::Format("\033[{}C", cardComponents->at(cardIndex)->GetWidth());
		}
		o_renderStream << utils::Format("\033[{}B\033[{}D", cardComponents->at(0)->GetHeight(), *width);
	}
}

void StageRenderStyle::OnThisRenderStyleApplied() const
{
	const CardComponetsT* cardComponents = GetCardComponents();
	if (!cardComponents)
	{
		return;
	}
	m_previousCardRenderStyle = cardComponents->at(0)->RetrieveRenderStyle();
	for (const auto& cardComponent : *cardComponents)
	{
		cardComponent->SetRenderStyle(m_cardRenderStyle->Clone());
	}
}

void StageRenderStyle::OnThisRenderStyleUnapplied() const
{
	const CardComponetsT* cardComponents = GetCardComponents();
	if (!cardComponents)
	{
		return;
	}
	for (const auto& cardComponent : *cardComponents)
	{
		cardComponent->SetRenderStyle(m_previousCardRenderStyle->Clone());
	}
	m_previousCardRenderStyle.reset();
}

utils::unique_ref<IRenderStyle> StageRenderStyle::Clone() const
{
	return utils::make_unique<StageRenderStyle>(m_cardRenderStyle->Clone());
}

const StageRenderStyle::CardComponetsT* StageRenderStyle::GetCardComponents() const
{
	const CardComponetsT* cardComponents = nullptr;
	utils::attribute* cardComponentsAttr = GetAttribute("cardComponents");
	if (!cardComponentsAttr)
	{
		return nullptr;
	}
	cardComponentsAttr->get(cardComponents).assertSuccess();
	if (cardComponents->empty())
	{
		return nullptr;
	}

	return cardComponents;
}
