#pragma once
#include <Components/IAttributesMap.h>

class IUIComponent;
struct UIContext;
struct DisplayInfo;
enum class ActionCode : uint8_t;
enum class UIComponentType : uint8_t;

class IUIManager : public IAttributesMap
{
public:
	DeclareScopedEnum(ErrorCode, uint8_t,
		UnmappedUIComponentType,
		UnregisteredComponent,
		AlreadyRegisteredComponent)
	using ErrorT = utils::Error<ErrorCode>;
	using ResultT = utils::Result<void, ErrorT>;
	using GetUIComponentResultT = utils::Result<const IUIComponent*, ErrorT>;

public:
	virtual ~IUIManager() = default;
	virtual const UIContext& GetUIContext() const = 0;
	virtual ResultT RegisterUIComponent(const IUIComponent&) = 0;
	virtual ResultT UnregisterUIComponent(const IUIComponent&) = 0;
	virtual ResultT MapUIComponent(const IUIComponent&, const UIComponentType&) = 0;
	virtual ResultT UnmapUIComponent(const UIComponentType&) = 0;
	virtual GetUIComponentResultT GetUIComponent(const UIComponentType&) const = 0;
	virtual const DisplayInfo& GetDisplayInfo() const = 0;
	virtual std::vector<std::string> GetInputActionMap(ActionCode) const = 0;
};
DefineScopeEnumOperatorImpl(ErrorCode, IUIManager)