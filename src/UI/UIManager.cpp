#include "stdafx.h"
#include "UI/UIManager.h"
#include "Components/AttributesMap.h"
#include "Control/ActionCode.h"
#include "Control/IInputRelay.h"
#include "UI/IUIComponent.h"
#include "DisplayInfo.h"
#include "Gameplay/UI/UIComponentsEnum.h"
#include <TypeVisitor.h>

#include "attribute.h"

#include "details/platforms.h"
#if defined(USE_POSIX_API)
#include <sys/ioctl.h>
#endif

namespace
{
utils::unique_ref<DisplayInfo> GetDisplayInfo()
{
	int width = 0, height = 0;
#if defined(USE_WIN32_API)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	width = w.ws_col;
	height = w.ws_row;
#endif
	return utils::make_unique<DisplayInfo>(width, height);
}
}

UIManager::UIManager(utils::IMessageQueue& i_thisFrameQueue, utils::IMessageQueue& i_nextFrameQueue, utils::IRecursiveControl& i_recursiveControl, const utils::SystemClock& i_systemClock, WindowManager& i_windowManager)
	: m_uiContext(utils::make_unique<UIContext>(i_thisFrameQueue, i_nextFrameQueue, i_recursiveControl, i_systemClock, i_windowManager, *this))
	, m_displayInfo(::GetDisplayInfo())
	, m_attributesMap(utils::make_unique<AttributesMap>())
{
	m_inputActionMap.emplace(ActionCode::Back, std::vector<std::string>{"\b", "\177"});
	m_inputActionMap.emplace(ActionCode::Enter, std::vector<std::string>{"\r", "\n", "\0"});
	m_inputActionMap.emplace(ActionCode::Pause, std::vector<std::string>{"p"});
	for (UIComponentType currentType = UIComponentType::_FIRST; currentType <= UIComponentType::_LAST; ++currentType)
	{
		m_uiComponentTypeMap.emplace(currentType, utils::Format("{}", currentType));
	}
}

UIManager::~UIManager() = default;

IUIManager::ResultT UIManager::RegisterUIComponent(const IUIComponent& i_uiComponent)
{
	auto componentFoundIt = std::find_if(m_uiComponents.begin(), m_uiComponents.end(),
		[&i_uiComponent](const IUIComponent* uiComponent)
		{
			return uiComponent == &i_uiComponent;
		});
	if (componentFoundIt != m_uiComponents.end())
	{
		return make_error<ErrorT>(ErrorCode::AlreadyRegisteredComponent);
	}
	i_uiComponent.OnShow();
	m_uiComponents.push_back(&i_uiComponent);
	return utils::Ok();
}

IUIManager::ResultT UIManager::UnregisterUIComponent(const IUIComponent& i_uiComponent)
{
	auto componentFoundIt = std::find_if(m_uiComponents.begin(), m_uiComponents.end(),
		[&i_uiComponent](const IUIComponent* uiComponent)
		{
			return uiComponent == &i_uiComponent;
		});
	if (componentFoundIt == m_uiComponents.end())
	{
		return make_error<ErrorT>(ErrorCode::UnregisteredComponent);
	}
	i_uiComponent.OnHide();
	m_uiComponents.erase(componentFoundIt);
	return utils::Ok();
}

IUIManager::ResultT UIManager::MapUIComponent(const IUIComponent& i_uiComponent, const UIComponentType& i_componentType)
{
	std::string key = m_uiComponentTypeMap[i_componentType];
	if (key.empty())
	{
		return make_error<IUIManager::ErrorT>(IUIManager::ErrorCode::UnmappedUIComponentType, "{} missing to map key", i_componentType);
	}
	BindAttribute(key, utils::attribute::make_bind_attribute(i_uiComponent, {nullptr}));
	return utils::Ok();
}

IUIManager::ResultT UIManager::UnmapUIComponent(const UIComponentType& i_componentType)
{
	std::string key = m_uiComponentTypeMap[i_componentType];
	if (key.empty())
	{
		return make_error<IUIManager::ErrorT>(IUIManager::ErrorCode::UnmappedUIComponentType, "{} missing to map key", i_componentType);
	}
	UnbindAttribute(key);
	return utils::Ok();
}

IUIManager::GetUIComponentResultT UIManager::GetUIComponent(const UIComponentType& i_componentType) const
{
	auto keyIt = m_uiComponentTypeMap.find(i_componentType);
	if (keyIt == m_uiComponentTypeMap.end())
	{
		return make_error<IUIManager::ErrorT>(IUIManager::ErrorCode::UnmappedUIComponentType, "{} missing to map key", i_componentType);
	}
	utils::attribute* uiAttribute = GetAttribute(keyIt->second);
	if (!uiAttribute)
	{
		return make_error<IUIManager::ErrorT>(IUIManager::ErrorCode::UnregisteredComponent, "{} is not mapped yet", i_componentType);
	}

	const IUIComponent* mappedUIComponent = nullptr;
	uiAttribute->get(mappedUIComponent).assertSuccess();
	return mappedUIComponent;
}

void UIManager::Render(RendererT& o_renderStream) const
{
	for (const IUIComponent* uiComponent : m_uiComponents)
	{
		uiComponent->Render(o_renderStream);
	}
}

const UIContext& UIManager::GetUIContext() const
{
	return *m_uiContext;
}

const DisplayInfo& UIManager::GetDisplayInfo() const
{
	return *m_displayInfo;
}

bool UIManager::ProcessInput(const std::string& input)
{
	for (const IUIComponent* uiComponent : m_uiComponents)
	{
		if (const IInputRelay* inputRelay = dynamic_cast<const IInputRelay*>(uiComponent))
		{
			if (inputRelay->ProcessInput(input))
			{
				break;
			}
		}
	}

	return true;
}

std::vector<std::string> UIManager::GetInputActionMap(ActionCode i_actionCode) const
{
	auto mapFoundIt = m_inputActionMap.find(i_actionCode);
	return mapFoundIt != m_inputActionMap.end() ? mapFoundIt->second : std::vector<std::string>{};
}

void UIManager::BindAttribute(const std::string& i_key, const utils::attribute& i_attribute)
{
	m_attributesMap->BindAttribute(i_key, i_attribute);
}

utils::unique_ptr<utils::attribute> UIManager::UnbindAttribute(const std::string& i_key)
{
	return m_attributesMap->UnbindAttribute(i_key);
}

void UIManager::ResetAttributes()
{
	m_attributesMap->ResetAttributes();
}

utils::attribute* UIManager::GetAttribute(const std::string& i_key) const
{
	return m_attributesMap->GetAttribute(i_key);
}

utils::attribute& UIManager::GetAttributeOrDefault(const std::string& i_key, utils::attribute& i_default) const
{
	return m_attributesMap->GetAttributeOrDefault(i_key, i_default);
}
