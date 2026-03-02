#pragma once

enum class UIComponentType : uint8_t;

class IRenderStyle;

class IUIRenderStyleFactory
{
public:
	struct UIRenderStyleHolder
	{
		UIComponentType componentType;
		utils::unique_ref<IRenderStyle> uiRenderStyle;
	};

public:
	virtual ~IUIRenderStyleFactory() = default;
	virtual std::vector<UIRenderStyleHolder> CreateUIRenderStyles() = 0;
	virtual std::string GetName() const = 0;
};