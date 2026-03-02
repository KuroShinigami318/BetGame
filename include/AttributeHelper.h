#pragma once
#include "attribute.h"

template <typename T>
void GetAttributeValue(std::optional<utils::attribute::result_t>& o_getResult, const utils::attribute& i_attribute, const T*& o_value)
{
	if (o_getResult.has_value() && o_getResult->isErr())
	{
		return;
	}
	o_getResult = i_attribute.get(o_value);
	o_getResult->assertSuccess();
}