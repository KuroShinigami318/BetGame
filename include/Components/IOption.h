#pragma once
#include "intrusive_list_tag.h"

class IOption : private utils::intrusive_list_tag
{
    friend class InputSelection;
    using intrusive_list_tag::next;
    using intrusive_list_tag::prev;
public:
    virtual ~IOption() = default;
    virtual const std::string& GetText() const = 0;
};