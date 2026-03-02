#pragma once

#include "ILogicGameplayFactory.h"
#include "Gameplay/Logic/StageLogic.h"

class BasicLogicGameplayFactory : public ILogicGameplayFactory
{
public:
	BasicLogicGameplayFactory(utils::unique_ref<LogicConfig> i_logicConfig);
	utils::unique_ref<IStageLogic> CreateStageLogic() override;

private:
	StageLogic m_stageLogic;
};