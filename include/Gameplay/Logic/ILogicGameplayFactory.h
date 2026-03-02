#pragma once

class IStageLogic;

class ILogicGameplayFactory
{
public:
	virtual ~ILogicGameplayFactory() = default;
	virtual utils::unique_ref<IStageLogic> CreateStageLogic() = 0;
};