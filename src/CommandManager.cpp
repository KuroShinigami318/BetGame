#include <stdafx.h>
#include <CommandManager.h>
#include <Components/ICommand.h>
#include <AsyncScopedHelper.h>

namespace
{
struct CommandHistoryItem : public utils::intrusive_list_tag
{
	utils::unique_ref<ICommand> command;
	using intrusive_list_tag::next;
	using intrusive_list_tag::prev;

	CommandHistoryItem(utils::unique_ref<ICommand> i_command)
		: command(std::move(i_command))
	{}
};

void DestroyCommandsFrom(utils::intrusive_list_tag* command)
{
	while (command)
	{
		utils::intrusive_list_tag* nextCommand = static_cast<CommandHistoryItem*>(command)->next;
		delete command;
		command = nextCommand;
	}
}
}

CommandManager::CommandManager()
	: m_cleanupThread(utils::thread_config{.thread_name = "CommandCleanup thread"})
	, m_asyncScopedHelper(utils::make_unique<utils::AsyncScopedHelper>())
{
	m_commandHistory.end(&m_headTag);
}

void CommandManager::Execute(utils::unique_ref<ICommand> i_command)
{
	ResetCommandHistory();
	i_command->Execute();
	m_currentCommand = &m_commandHistory.push_back(*new CommandHistoryItem{ std::move(i_command) });
}

void CommandManager::ExecuteAndForget(utils::unique_ref<ICommand> i_command)
{
	m_currentCommand = &m_headTag;
	ResetCommandHistory();
	i_command->Execute();
}

bool CommandManager::Undo()
{
	if (m_currentCommand != &m_headTag)
	{
		CommandHistoryItem* currentCommandItem = static_cast<CommandHistoryItem*>(m_currentCommand);
		currentCommandItem->command->Undo();
		m_currentCommand = currentCommandItem->prev;
		return true;
	}
	return false;
}

bool CommandManager::Redo()
{
	CommandHistoryItem* nextCommandItem = static_cast<CommandHistoryItem*>(static_cast<CommandHistoryItem*>(m_currentCommand)->next);
	if (nextCommandItem != &m_headTag)
	{
		nextCommandItem->command->Execute();
		m_currentCommand = nextCommandItem;
		return true;
	}
	return false;
}

void CommandManager::RemoveCommandsFrom(utils::intrusive_list_tag& i_commandItem)
{
	if (&i_commandItem == &m_headTag)
	{
		return;
	}
	m_commandHistory.remove(i_commandItem, m_commandHistory.end().current);
	m_asyncScopedHelper->StartMandatoryTask(m_cleanupThread, &DestroyCommandsFrom, &i_commandItem);
}

void CommandManager::ResetCommandHistory()
{
	RemoveCommandsFrom(*static_cast<CommandHistoryItem*>(m_currentCommand)->next);
}