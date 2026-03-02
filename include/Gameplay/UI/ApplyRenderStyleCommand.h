#pragma once
#include <Components/ICommand.h>

class IUIManager;
class IUIRenderStyleFactory;

class ApplyRenderStyleCommand : public ICommand
{
public:
	ApplyRenderStyleCommand(IUIManager& i_uiManager, utils::unique_ref<IUIRenderStyleFactory> i_factory);
	void Execute() override;
	void Undo() override;

private:
	IUIManager& m_uiManager;
	utils::unique_ref<IUIRenderStyleFactory> m_factory;
	utils::unique_ptr<IUIRenderStyleFactory> m_oldFactory;
};