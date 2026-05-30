#pragma once

#include "IWindow.h"
#include "Components/SwitchContainer.h"
#include "Control/InputRelay.h"
#include "UI/visibility_flag.h"

class WindowManager;
struct ClosedByBackActionTag {};

class WindowBase : public IWindow, public InputRelay, public SwitchContainer
{
public:
	WindowBase(const UIContext& i_uiContext);
	WindowBase(const WindowBase&);
	WindowBase(WindowBase&&) noexcept;
	WindowBase& operator=(const WindowBase&);
	WindowBase& operator=(WindowBase&&) noexcept;
	OpenResultT Open() override;
	void Close(const ResultT&) override;
	bool IsOpened() const override;
	void OnShow() const override;
	void OnHide() const override;
	bool ProcessInput(const std::string& input) const override;
	bool ProcessInputImpl(const std::string& input) const override;

protected:
	mutable utils::flag_set<visibility_flag> m_currentVisibility;
	
private:
	utils::unique_ptr<ResultT> m_result;
};