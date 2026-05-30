#pragma once

struct UIContext;
class IWindow;

namespace utils
{
class AsyncScopedHelper;
}

class StartGameFlow
{
public:
	StartGameFlow(IWindow& i_splashscreenWindow);

private:
	void StartGame();

private:
	IWindow& m_splashscreenWindow;
	utils::unique_ref<utils::AsyncScopedHelper> m_asyncScopedHelper;
};