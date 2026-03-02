#pragma once

class IUIComponent;
class IStageLogic;
struct UIContext;

enum class UIComponentType : uint8_t;

class IUIGameplayFactory
{
public:
	struct UIHolder
	{
		UIComponentType componentType;
		utils::unique_ref<IUIComponent> uiComponent;
	};

public:
	virtual ~IUIGameplayFactory() = default;
	virtual std::vector<UIHolder> CreateUIComponents(const UIContext&, IStageLogic&) = 0;
	virtual std::string GetName() const = 0;
};