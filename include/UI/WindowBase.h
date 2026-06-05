#pragma once

#include "IWindow.h"
#include "IInputHints.h"
#include "Components/SwitchContainer.h"
#include "Control/InputRelay.h"
#include "UI/visibility_flag.h"

class WindowManager;
struct ClosedByBackActionTag {};

class WindowBase : public IWindow, public InputRelay, public SwitchContainer, public IInputHints
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
	void InitializeInputHints(IInputHints& i_inputHints) const override;
	void AddHint(ActionCode i_actionCode, const std::string& i_hint, utils::CallableBound<bool(const std::string&)> i_processInputFunc) override;

private:
	void ClearHints();
	void OnActiveComponentChanged(const IUIComponent& i_activeComponent);

protected:
	mutable utils::flag_set<visibility_flag> m_currentVisibility;
	
private:
	utils::unique_ptr<ResultT> m_result;
	std::unordered_map<ActionCode, utils::unique_ref<struct InputHint>> m_inputHintsMap;
	std::vector<utils::unique_ref<class InputHintRelay>> m_inputHintRelays;
	utils::Connection m_activeComponentChangedConnection;
	IUIComponent* m_currentDisplayedHints = nullptr;
};