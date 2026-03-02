#include <stdafx.h>
#include <Gameplay/UI/ApplyRenderStyleCommand.h>
#include <Gameplay/UI/IUIRenderStyleFactory.h>
#include <UI/IUIManager.h>
#include <UI/UIHelper.h>

#include <attribute.h>

ApplyRenderStyleCommand::ApplyRenderStyleCommand(IUIManager& i_uiManager, utils::unique_ref<IUIRenderStyleFactory> i_factory)
	: m_uiManager(i_uiManager)
	, m_factory(std::move(i_factory))
{
}

void ApplyRenderStyleCommand::Execute()
{
	if (m_factory.valueless_after_move())
	{
		return;
	}
	const UniqueHolder<IUIRenderStyleFactory>* currentFactory = nullptr;
	utils::attribute* oldFactoryAttribute = m_uiManager.GetAttribute(UI_RENDER_STYLE_FACTORY_ATTRIBUTE_KEY);
	if (oldFactoryAttribute)
	{
		oldFactoryAttribute->get(currentFactory).assertSuccess();
		m_oldFactory = std::move(const_cast<UniqueHolder<IUIRenderStyleFactory>*>(currentFactory)->value);
	}

	ApplyRenderStyle(m_uiManager, *m_factory);

	m_uiManager.BindAttribute(UI_RENDER_STYLE_NAME_ATTRIBUTE_KEY, utils::attribute::make_attribute<std::string>(m_factory->GetName()));
	m_uiManager.BindAttribute(UI_RENDER_STYLE_FACTORY_ATTRIBUTE_KEY, utils::attribute::make_attribute<UniqueHolder<IUIRenderStyleFactory>>(std::move(m_factory)));
}

void ApplyRenderStyleCommand::Undo()
{
	swap(m_factory, m_oldFactory);
	Execute();
	swap(m_factory, m_oldFactory);
}