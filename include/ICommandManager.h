#pragma once

class ICommand;

class ICommandManager
{
public:
	virtual ~ICommandManager() = default;
	virtual void Execute(utils::unique_ref<ICommand> i_command) = 0;
	virtual void ExecuteAndForget(utils::unique_ref<ICommand> i_command) = 0;
	virtual bool Undo() = 0;
	virtual bool Redo() = 0;
};