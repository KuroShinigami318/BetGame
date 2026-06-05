#include "stdafx.h"
#include "UI/WindowBase.h"
#include "Control/ActionCode.h"
#include "UI/IUIManager.h"
#include "UI/WindowManager.h"
#include "UI/IWindowError.h"
#include "UI/UIHelper.h"

struct InputHint
{
	ActionCode actionCode;
	std::string hint;
	utils::CallableBound<bool(const std::string&)> processInputFunc;
	bool isFocused = false;

	InputHint(ActionCode i_actionCode, const std::string& i_hint, utils::CallableBound<bool(const std::string&)> i_processInputFunc)
		: actionCode(i_actionCode)
		, hint(i_hint)
		, processInputFunc(i_processInputFunc)
	{
	}

	void SetFocus()
	{
		isFocused = true;
	}

	std::string GetDisplayHint() const
	{
		utils::Epilogue resetFocus([this](){ const_cast<bool&>(isFocused) = false; });
		if (isFocused)
		{
			return FormatText(hint.c_str(), utils::Log::TextFormat(utils::Log::TextStyle::Bold, utils::RGBColor(0, 255, 0)));
		}
		return hint;
	}
};

class InputHintRelay : public IInputRelay
{
public:
	InputHintRelay(InputHint& i_inputHint, IUIManager& i_uiManager) : m_inputHint(i_inputHint), m_uiManager(i_uiManager) {}
	bool ProcessInput(const std::string& input) const override
	{
		if ((m_inputHint.actionCode == ActionCode::Custom && m_inputHint.processInputFunc(input))
			|| m_uiManager.IsInputAction(input, m_inputHint.actionCode))
		{
			m_inputHint.SetFocus();
		}
		return false;
	}

private:
	InputHint& m_inputHint;
	IUIManager& m_uiManager;
};

namespace
{
class InputHintsText : public IUIComponent
{
private:
	using InputHintsMapT = std::unordered_map<ActionCode, utils::unique_ref<struct InputHint>>;

public:
	InputHintsText(const UIContext& i_uiContext, const InputHintsMapT& i_inputHintsMap) : IUIComponent(i_uiContext), m_inputHintsMap(i_inputHintsMap) {}

	void Render(RendererT& o_renderStream) const override
	{
		for (const auto& [_, inputHint] : m_inputHintsMap)
		{
			o_renderStream << inputHint->GetDisplayHint() << "  ";
		}
		o_renderStream << std::endl << "\033[1G";
	}

	utils::unique_ref<IComponent> Clone() override
	{
		return utils::make_unique<InputHintsText>(GetUIContext(), m_inputHintsMap);
	}

private:
	const InputHintsMapT& m_inputHintsMap;
};
}

WindowBase::WindowBase(const UIContext& i_uiContext)
	: IUIComponent(i_uiContext)
	, SwitchContainer(i_uiContext)
{
	m_activeComponentChangedConnection = sig_onActiveComponentChanged.Connect(&WindowBase::OnActiveComponentChanged, this);
}

WindowBase::WindowBase(const WindowBase& other)
	: IUIComponent(other.GetUIContext())
	, SwitchContainer(other)
	, m_currentVisibility(other.m_currentVisibility)
{
	ASSERT(!other.IsOpened(), "Copying an opened window is not allowed");
	m_activeComponentChangedConnection = sig_onActiveComponentChanged.Connect(&WindowBase::OnActiveComponentChanged, this);
}

WindowBase::WindowBase(WindowBase&& other) noexcept
	: IUIComponent(other.GetUIContext())
	, SwitchContainer(std::move(other))
	, m_currentVisibility(std::move(other.m_currentVisibility))
{
	m_activeComponentChangedConnection = sig_onActiveComponentChanged.Connect(&WindowBase::OnActiveComponentChanged, this);
}

WindowBase& WindowBase::operator=(const WindowBase& other)
{
	if (this != &other)
	{
		ASSERT(!other.IsOpened(), "Copying an opened window is not allowed");
		SwitchContainer::operator=(other);
		m_currentVisibility = other.m_currentVisibility;
		m_activeComponentChangedConnection = sig_onActiveComponentChanged.Connect(&WindowBase::OnActiveComponentChanged, this);
	}
	return *this;
}

WindowBase& WindowBase::operator=(WindowBase&& other) noexcept
{
	if (this != &other)
	{
		SwitchContainer::operator=(std::move(other));
		m_currentVisibility = std::move(other.m_currentVisibility);
		m_activeComponentChangedConnection = sig_onActiveComponentChanged.Connect(&WindowBase::OnActiveComponentChanged, this);
	}
	return *this;
}

IWindow::OpenResultT WindowBase::Open()
{
	m_result.reset();
	InteractiveComponentTag* interactiveTag;
	GetFirstInteractive(interactiveTag);
	if (!IsEnd(interactiveTag))
	{
		SetActiveComponent(*GetUIComponent(interactiveTag));
	}
	if (m_inputHintsMap.empty())
	{
		InitializeInputHints(*this);
	}
	m_uiContext.windowManager.RegisterWindow(*this);
	auto enterRecursionResult = m_uiContext.recursiveControl.enter(HashObject(this));
	if (enterRecursionResult.isErr())
	{
		return make_inner_error<IWindowError>(IWindowErrorCode::RecursiveControlError, enterRecursionResult.unwrapErr());
	}

	return const_cast<const ResultT*>(m_result.get());
}

void WindowBase::Close(const ResultT& i_result)
{
	ASSERT(m_result == nullptr, "Closing a window more than once is not allowed");
	ClearInputRelays();
	m_uiContext.windowManager.UnregisterWindow(*this);
	m_uiContext.recursiveControl.exit(HashObject(this));
	m_currentVisibility.clear();
	m_result = utils::make_unique<ResultT>(i_result);
}

bool WindowBase::IsOpened() const
{
	return !m_currentVisibility.empty();
}

void WindowBase::OnShow() const
{
	m_currentVisibility.set(visibility_flag::visible);
	m_currentVisibility.unset(visibility_flag::invisible);
}

void WindowBase::OnHide() const
{
	m_currentVisibility.set(visibility_flag::invisible);
	m_currentVisibility.unset(visibility_flag::visible);
}

bool WindowBase::ProcessInput(const std::string& input) const
{
	return InputRelay::ProcessInput(input);
}

bool WindowBase::ProcessInputImpl(const std::string& input) const
{
	bool processed = SwitchContainer::ProcessInput(input);
	if (!processed && (processed = m_uiContext.uiManager.IsInputAction(input, ActionCode::Back)))
	{
		const_cast<WindowBase&>(*this).Close(ClosedByBackActionTag{});
	}

	return processed;
}

void WindowBase::InitializeInputHints(IInputHints& i_inputHints) const
{
	WindowBase* self = const_cast<WindowBase*>(this);
	SwitchContainer::InitializeInputHints(i_inputHints);
	i_inputHints.AddHint(ActionCode::Back, "[Esc]: Close Window");
	for (auto& [_, inputHint] : m_inputHintsMap)
	{
		utils::unique_ref<InputHintRelay> relay = utils::make_unique<InputHintRelay>(*inputHint, m_uiContext.uiManager);
		self->AddInputRelay(*relay);
		self->m_inputHintRelays.push_back(std::move(relay));
	}
	if (!m_inputHintsMap.empty())
	{
		self->m_currentDisplayedHints = &self->AddUIComponent(utils::make_unique<InputHintsText>(GetUIContext(), m_inputHintsMap));
	}
}

void WindowBase::AddHint(ActionCode i_actionCode, const std::string& i_hint, utils::CallableBound<bool(const std::string&)> i_processInputFunc)
{
	if (m_inputHintsMap.contains(i_actionCode))
	{
		return;
	}
	m_inputHintsMap.insert_or_assign(i_actionCode, utils::make_unique<InputHint>(i_actionCode, i_hint, i_processInputFunc));
}

void WindowBase::ClearHints()
{
	if (m_currentDisplayedHints)
	{
		RetrieveUIComponent(*m_currentDisplayedHints).assertSuccess();
		m_currentDisplayedHints = nullptr;
	}
	for (utils::unique_ref<InputHintRelay>& relay : m_inputHintRelays)
	{
		RemoveInputRelay(*relay);
	}
	m_inputHintRelays.clear();
	m_inputHintsMap.clear();
}

void WindowBase::OnActiveComponentChanged(const IUIComponent& i_activeComponent)
{
	ClearHints();
	InitializeInputHints(*this);
}