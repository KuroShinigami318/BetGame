#include "stdafx.h"
#include "UI/WindowBase.h"
#include "Control/ActionCode.h"
#include "UI/IUIManager.h"
#include "UI/IWindowError.h"
#include "UI/WindowManager.h"

void WindowBase::Open()
{
	m_uiContext.windowManager.RegisterWindow(*this);
	m_uiContext.recursiveControl.enter(HashObject(this)).assertSuccess();
}

void WindowBase::Close()
{
	ClearInputRelays();
	m_uiContext.windowManager.UnregisterWindow(*this);
	m_uiContext.recursiveControl.exit(HashObject(this));
	m_currentVisibility.clear();
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

IUIComponent& WindowBase::AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent)
{
	if (const IInputRelay* inputRelay = dynamic_cast<const IInputRelay*>(i_uiComponent.get()))
	{
		AddInputRelay(*inputRelay);
	}
	return *m_uiComponents.emplace_back(std::move(i_uiComponent));
}

IWindow::RetrieveResult WindowBase::RetrieveUIComponent(IUIComponent& i_uiComponent)
{
	auto foundComponentIt = std::find(m_uiComponents.begin(), m_uiComponents.end(), &i_uiComponent);
	if (foundComponentIt == m_uiComponents.end())
	{
		return make_error<IWindowError>(IWindowErrorCode::ComponentNotFound);
	}
	utils::unique_ref<IUIComponent> retrievedComponent = std::move(*foundComponentIt);
	m_uiComponents.erase(foundComponentIt);
	return retrievedComponent;
}

void WindowBase::Render(RendererT& o_renderStream) const
{
	for (const auto& uiComponent : m_uiComponents)
	{
		uiComponent->Render(o_renderStream);
	}
}

bool WindowBase::ProcessInputImpl(const std::string& input) const
{
	if (m_uiContext.uiManager.IsInputAction(input, ActionCode::Back))
	{
		const_cast<WindowBase&>(*this).Close();
		return true;
	}

	return false;
}
