#pragma once
#include "IAttributesMap.h"

class AttributesMap : virtual public IAttributesMap
{
public:
	AttributesMap() = default;
	AttributesMap(const AttributesMap&);
	AttributesMap& operator=(const AttributesMap&);
	void BindAttribute(const std::string& i_key, const utils::attribute& i_attribute) override;
	utils::unique_ptr<utils::attribute> UnbindAttribute(const std::string& i_key) override;
	void ResetAttributes() override;
	utils::attribute* GetAttribute(const std::string& i_key) const override;
	utils::attribute& GetAttributeOrDefault(const std::string& i_key, utils::attribute& i_default) const override;

private:
	std::unordered_map<std::string, utils::unique_ref<utils::attribute>> m_attributes;
};