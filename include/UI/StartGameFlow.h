#pragma once
#include "Control/IInputRelay.h"

struct UIContext;
class ICommandManager;
class IWindow;

namespace utils
{
class AsyncScopedHelper;
}

class StartGameFlow : public IInputRelay
{
public:
	StartGameFlow(IWindow& i_splashscreenWindow, ICommandManager& i_commandManager);
	bool ProcessInput(const std::string& i_input) const override;

private:
	void StartGame();

private:
	IWindow& m_splashscreenWindow;
	ICommandManager& m_commandManager;
	mutable bool m_isEnterPressed;
	utils::unique_ref<utils::AsyncScopedHelper> m_asyncScopedHelper;
};