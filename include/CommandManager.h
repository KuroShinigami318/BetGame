#pragma once
#include <ICommandManager.h>
#include <intrusive_list.h>
#include <intrusive_list_tag.h>

class CommandManager : public ICommandManager, public utils::noncopy
{
public:
	CommandManager();
	void Execute(utils::unique_ref<ICommand> i_command) override;
	void ExecuteAndForget(utils::unique_ref<ICommand> i_command) override;
	bool Undo() override;
	bool Redo() override;

private:
	void RemoveCommandsFrom(utils::intrusive_list_tag& i_commandItem);
	void ResetCommandHistory();

private:
	utils::intrusive_list m_commandHistory;
	utils::intrusive_list_tag m_headTag;
	utils::intrusive_list_tag* m_currentCommand = &m_headTag;
	utils::message_thread m_cleanupThread;
	utils::unique_ref<utils::AsyncScopedHelper> m_asyncScopedHelper;
};