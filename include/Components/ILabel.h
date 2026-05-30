#pragma once
#include "IComponent.h"

class ILabel : virtual public IComponent
{
public:
    virtual void SetText(const std::string& i_text) = 0;
    virtual const std::string& GetText() const = 0;
};