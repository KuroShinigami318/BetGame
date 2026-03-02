#pragma once

namespace utils
{
class attribute;
}

class IAttributesMap
{
public:
	virtual ~IAttributesMap() = default;
	virtual void BindAttribute(const std::string& i_key, const utils::attribute& i_attribute) = 0;
	virtual utils::unique_ptr<utils::attribute> UnbindAttribute(const std::string& i_key) = 0;
	virtual void ResetAttributes() = 0;
	virtual utils::attribute* GetAttribute(const std::string& i_key) const = 0;
	virtual utils::attribute& GetAttributeOrDefault(const std::string& i_key, utils::attribute& i_default) const = 0;
};