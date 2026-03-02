#pragma once

#include "Gameplay/UI/IUIGameplayFactory.h"
#include "random_generator.h"

class IStage;
class IBid;

class BasicUIGameplayFactory : public IUIGameplayFactory
{
public:
	std::vector<UIHolder> CreateUIComponents(const UIContext&, IStageLogic&) override;
	std::string GetName() const override;
	static std::string GetStaticName();

private:
	utils::unique_ref<IStage> CreateStageComponent(const UIContext&, IStageLogic&);
	utils::unique_ref<IBid> CreateBidComponent(const UIContext&, IStageLogic&);
	utils::RandomGenerator<uint16_t, 3, 9> m_randomGenerator;
};