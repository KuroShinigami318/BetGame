#include "stdafx.h"
#include "UI/WindowBase.h"
#include "Control/ActionCode.h"
#include "UI/IUIManager.h"
#include "UI/WindowManager.h"
#include "UI/IWindowError.h"

WindowBase::WindowBase(const UIContext& i_uiContext)
	: IUIComponent(i_uiContext)
	, SwitchContainer(i_uiContext)
{
}

WindowBase::WindowBase(const WindowBase& other)
	: IUIComponent(other.GetUIContext())
	, SwitchContainer(other)
	, m_currentVisibility(other.m_currentVisibility)
{
	ASSERT(!other.IsOpened(), "Copying an opened window is not allowed");
}

WindowBase::WindowBase(WindowBase&& other) noexcept
	: IUIComponent(other.GetUIContext())
	, SwitchContainer(std::move(other))
	, m_currentVisibility(std::move(other.m_currentVisibility))
{
}

WindowBase& WindowBase::operator=(const WindowBase& other)
{
	if (this != &other)
	{
		SwitchContainer::operator=(other);
		m_currentVisibility = other.m_currentVisibility;
		ASSERT(!other.IsOpened(), "Copying an opened window is not allowed");
	}
	return *this;
}

WindowBase& WindowBase::operator=(WindowBase&& other) noexcept
{
	if (this != &other)
	{
		SwitchContainer::operator=(std::move(other));
		m_currentVisibility = std::move(other.m_currentVisibility);
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
