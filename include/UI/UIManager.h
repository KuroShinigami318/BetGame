#pragma once
#include "IUIManager.h"
#include "RendererType.h"

namespace utils
{
class IMessageQueue;
class IRecursiveControl;
struct SystemClock;
}

class AttributesMap;
class WindowManager;
class ICommandManager;
class IUIManager;

class UIManager : public IUIManager
{
public:
	UIManager(utils::IMessageQueue& i_thisFrameQueue, utils::IMessageQueue& i_nextFrameQueue, utils::IRecursiveControl& i_recursiveControl, const utils::SystemClock& i_systemClock, WindowManager& i_windowManager, ICommandManager& i_commandManager);
	~UIManager();
	ResultT RegisterUIComponent(const IUIComponent&) override;
	ResultT UnregisterUIComponent(const IUIComponent&) override;
	ResultT MapUIComponent(const IUIComponent&, const UIComponentType&) override;
	ResultT UnmapUIComponent(const UIComponentType&) override;
	GetUIComponentResultT GetUIComponent(const UIComponentType&) const override;
	void Render(RendererT& o_renderStream) const;
	const UIContext& GetUIContext() const override;
	const DisplayInfo& GetDisplayInfo() const override;
	bool ProcessInput(const std::string& input);
	std::vector<std::string> GetInputActionMap(ActionCode) const override;
	bool IsInputAction(const std::string& i_inputAction, ActionCode i_actionCode) const override;

	// IAttributesMap implementation
	void BindAttribute(const std::string& i_key, const utils::attribute& i_attribute) override;
	utils::unique_ptr<utils::attribute> UnbindAttribute(const std::string& i_key) override;
	void ResetAttributes() override;
	utils::attribute* GetAttribute(const std::string& i_key) const override;
	utils::attribute& GetAttributeOrDefault(const std::string& i_key, utils::attribute& i_default) const override;

private:
	std::vector<const IUIComponent*> m_uiComponents;
	utils::unique_ref<UIContext> m_uiContext;
	utils::unique_ref<DisplayInfo> m_displayInfo;
	std::unordered_map<ActionCode, std::vector<std::string>> m_inputActionMap;
	std::unordered_map<UIComponentType, std::string> m_uiComponentTypeMap;
	utils::unique_ref<AttributesMap> m_attributesMap;
};