#pragma once

#include "IUIComponent.h"

enum class IWindowErrorCode : uint8_t;

class IWindow : public IUIComponent
{
public:
	using IWindowError = utils::Error<IWindowErrorCode>;
	using RetrieveResult = utils::Result<utils::unique_ref<IUIComponent>, IWindowError>;
	using IUIComponent::IUIComponent;
	virtual void Open() = 0;
	virtual void Close() = 0;
	virtual bool IsOpened() const = 0;
	virtual IUIComponent& AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent) = 0;
	virtual RetrieveResult RetrieveUIComponent(IUIComponent& i_uiComponent) = 0;
};