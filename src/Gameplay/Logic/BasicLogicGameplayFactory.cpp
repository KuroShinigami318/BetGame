#include "stdafx.h"
#include "Gameplay/Logic/BasicLogicGameplayFactory.h"

BasicLogicGameplayFactory::BasicLogicGameplayFactory(utils::unique_ref<LogicConfig> i_logicConfig)
	: m_stageLogic(std::move(i_logicConfig))
{
}

utils::unique_ref<IStageLogic> BasicLogicGameplayFactory::CreateStageLogic()
{
	return utils::unique_ref<IStageLogic>(&m_stageLogic, [](IStageLogic*) {});
}