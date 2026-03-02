#include "stdafx.h"
#include "Components/AttributesMap.h"
#include "attribute.h"

AttributesMap::AttributesMap(const AttributesMap& other)
{
	*this = other;
}

AttributesMap& AttributesMap::operator=(const AttributesMap& other)
{
	if (this != &other)
	{
		m_attributes.clear();
		for (auto& [key, value] : other.m_attributes)
		{
			m_attributes.emplace(key, utils::make_unique<utils::attribute>(*value));
		}
	}
	return *this;
}

void AttributesMap::BindAttribute(const std::string& i_key, const utils::attribute& i_attribute)
{
	m_attributes.insert_or_assign(i_key, utils::make_unique<utils::attribute>(i_attribute));
}

utils::unique_ptr<utils::attribute> AttributesMap::UnbindAttribute(const std::string& i_key)
{
	auto foundIter = m_attributes.find(i_key);
	if (foundIter == m_attributes.end())
	{
		return nullptr;
	}
	utils::unique_ptr<utils::attribute> extractedAttribute = std::move(foundIter->second);
	m_attributes.erase(foundIter);
	return extractedAttribute;
}

void AttributesMap::ResetAttributes()
{
	m_attributes.clear();
}

utils::attribute* AttributesMap::GetAttribute(const std::string& i_key) const
{
	auto foundIter = m_attributes.find(i_key);
	if (foundIter == m_attributes.end())
	{
		return nullptr;
	}
	return foundIter->second.get();
}

utils::attribute& AttributesMap::GetAttributeOrDefault(const std::string& i_key, utils::attribute& i_default) const
{
	if (utils::attribute* foundAttr = GetAttribute(i_key))
	{
		return *foundAttr;
	}

	return i_default;
}
