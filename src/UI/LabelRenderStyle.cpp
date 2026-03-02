#include "stdafx.h"
#include "UI/LabelRenderStyle.h"
#include "UI/UIHelper.h"
#include "AttributeHelper.h"

void LabelRenderStyle::Render(RendererT& i_renderer) const
{
	i_renderer << "\033[s";
	utils::Epilogue restoreCursorPosition([&i_renderer]() { i_renderer << "\033[u"; });
	utils::attribute* widthAttr = GetAttribute("width");
	utils::attribute* heightAttr = GetAttribute("height");
	utils::attribute* textAttr = GetAttribute("text");

	if (!widthAttr || !heightAttr || !textAttr)
	{
		return;
	}

	static utils::attribute line_boundary = utils::attribute::make_attribute<std::string>("*");
	static utils::attribute text_format = utils::attribute::make_attribute<utils::Log::TextFormat>();
	utils::attribute& horizontalLineBoundaryAttr = GetAttributeOrDefault("horizontal_line_boundary", line_boundary);
	utils::attribute& verticalLineBoundaryAttr = GetAttributeOrDefault("vertical_line_boundary", line_boundary);
	utils::attribute& textFormatAttr = GetAttributeOrDefault("text_format", text_format);

	const uint16_t* width = nullptr;
	const uint16_t* height = nullptr;
	const std::string* horizontalLineBoundary = nullptr;
	const std::string* verticalLineBoundary = nullptr;
	const std::string text = textAttr->get_converter().to_string();
	const utils::Log::TextFormat* textFormat = nullptr;

	std::optional<utils::attribute::result_t> getResult;
	GetAttributeValue(getResult, *widthAttr, width);
	GetAttributeValue(getResult, *heightAttr, height);
	GetAttributeValue(getResult, horizontalLineBoundaryAttr, horizontalLineBoundary);
	GetAttributeValue(getResult, verticalLineBoundaryAttr, verticalLineBoundary);
	GetAttributeValue(getResult, textFormatAttr, textFormat);
	if (getResult->isErr())
	{
		return;
	}

	std::string formattedText = FormatText(text.c_str(), *textFormat);
	std::string formattedHorizontalLineBoundary = FormatText(horizontalLineBoundary->c_str(), *textFormat);
	std::string formattedVerticalLineBoundary = FormatText(verticalLineBoundary->c_str(), *textFormat);

	for (uint16_t row = 0; row < *height; ++row)
	{
		for (uint16_t col = 0; col < *width; ++col)
		{
			if (row == 0 || row == *height - 1)
			{
				i_renderer << formattedHorizontalLineBoundary;
			}
			else if (col == 0 || col == *width - 1)
			{
				i_renderer << formattedVerticalLineBoundary;
			}
			else if (row == *height / 2 && (col == (*width - text.size()) / 2 || *width == text.size()))
			{
				i_renderer << formattedText;
				col += text.size() - 1;
			}
			else
			{
				i_renderer << " ";
			}
		}
		i_renderer << utils::Format("\033[u\033[s\033[{}B", row + 1);
	}
}

utils::unique_ref<IRenderStyle> LabelRenderStyle::Clone() const
{
	return utils::make_unique<LabelRenderStyle>(*this);
}
