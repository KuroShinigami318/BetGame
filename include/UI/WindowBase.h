#pragma once

#include "IWindow.h"
#include "Control/InputRelay.h"
#include "UI/visibility_flag.h"

class WindowManager;

class WindowBase : public IWindow, public InputRelay
{
public:
	using IWindow::IWindow;
	void Open() override;
	void Close() override;
	bool IsOpened() const override;
	void OnShow() const override;
	void OnHide() const override;
	IUIComponent& AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent) override;
	RetrieveResult RetrieveUIComponent(IUIComponent& i_uiComponent) override;
	void Render(RendererT& o_renderStream) const override;
	bool ProcessInputImpl(const std::string& input) const override;

protected:
	std::vector<utils::unique_ref<IUIComponent>> m_uiComponents;
	mutable utils::flag_set<visibility_flag> m_currentVisibility;
};