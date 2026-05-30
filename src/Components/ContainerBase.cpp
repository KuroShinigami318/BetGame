#include "stdafx.h"
#include "Components/ContainerBase.h"
#include "UI/IContainerError.h"
#include "intrusive_list.h"
#include "intrusive_list_tag.h"

struct InteractiveComponentTag : public utils::intrusive_list_tag
{
    using utils::intrusive_list_tag::next;
    using utils::intrusive_list_tag::prev;
    ComponentListTag* componentTag;
    InteractiveComponentTag(ComponentListTag* i_componentTag = nullptr) : componentTag(i_componentTag) {}
};

struct ComponentListTag : public utils::intrusive_list_tag
{
    using utils::intrusive_list_tag::next;
    using utils::intrusive_list_tag::prev;
    utils::unique_ptr<IUIComponent> component;
    IInputRelay* inputRelay = nullptr;
    InteractiveComponentTag interactiveTag{ this };
    ComponentListTag(utils::unique_ptr<IUIComponent> i_component = nullptr) : component(std::move(i_component)) {}
};

ContainerBase::~ContainerBase()
{
    m_uiComponentsList->deallocate_all();
}

ContainerBase::ContainerBase(const UIContext& i_uiContext)
    : IUIComponent(i_uiContext)
    , m_endTag(utils::make_unique<ComponentListTag>())
    , m_uiComponentsList(utils::make_unique<utils::intrusive_list>())
    , m_interactiveComponentsList(utils::make_unique<utils::intrusive_list>())
    , m_componentCount(0)
{
    m_uiComponentsList->end(m_endTag.get());
    m_interactiveComponentsList->end(&m_endTag->interactiveTag);
}

ContainerBase::ContainerBase(ContainerBase&& other) noexcept
    : IUIComponent(std::move(other))
    , m_endTag(utils::make_unique<ComponentListTag>())
    , m_uiComponentsList(utils::make_unique<utils::intrusive_list>())
    , m_interactiveComponentsList(utils::make_unique<utils::intrusive_list>())
    , m_componentCount(0)
{
    *this = std::move(other);
}

ContainerBase& ContainerBase::operator=(ContainerBase&& other) noexcept
{
    if (this != &other)
    {
        m_interactiveComponentsList->reset();
        m_uiComponentsList->deallocate_all();
        *m_uiComponentsList = std::move(*other.m_uiComponentsList);
        *m_interactiveComponentsList = std::move(*other.m_interactiveComponentsList);
        m_uiComponentsList->end(m_endTag.get());
        m_interactiveComponentsList->end(&m_endTag->interactiveTag);
        m_componentCount = std::exchange(other.m_componentCount, 0);
    }
    return *this;
}

ContainerBase& ContainerBase::operator=(const ContainerBase& other)
{
    if (this != &other)
    {
        m_interactiveComponentsList->reset(&m_endTag->interactiveTag);
        m_interactiveComponentsList->end(&m_endTag->interactiveTag);
        m_uiComponentsList->deallocate_all();
        other.m_uiComponentsList->traverse([this](utils::intrusive_list_tag& listTag)
        {
            ComponentListTag& componentHolder = static_cast<ComponentListTag&>(listTag);
            AddUIComponent(utils::dynamic_unique_cast<IUIComponent>(componentHolder.component->Clone()));
        });
        m_componentCount = other.m_componentCount;
    }
    return *this;
}

IUIComponent& ContainerBase::AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent)
{
    ComponentListTag* newComponentTag = new ComponentListTag(std::move(i_uiComponent));
    if (newComponentTag->inputRelay = dynamic_cast<IInputRelay*>(newComponentTag->component.get()))
    {
        m_interactiveComponentsList->push_back(newComponentTag->interactiveTag);
    }
    m_uiComponentsList->push_back(*newComponentTag);
    ++m_componentCount;
    return *newComponentTag->component;
}

IContainer::RetrieveResult ContainerBase::RetrieveUIComponent(IUIComponent& i_uiComponent)
{
    for (utils::intrusive_list_tag& listTag : *m_uiComponentsList)
    {
        ComponentListTag& componentHolder = static_cast<ComponentListTag&>(listTag);
        if (componentHolder.component.get() == &i_uiComponent)
        {
            utils::unique_ref<IUIComponent> retrievedComponent = std::move(componentHolder.component);
            if (componentHolder.inputRelay)
            {
                m_interactiveComponentsList->remove(componentHolder.interactiveTag);
            }
            delete &m_uiComponentsList->remove(componentHolder);
            --m_componentCount;
            return retrievedComponent;
        }
    }
    return make_error<IContainerError>(IContainerErrorCode::ComponentNotFound);
}

void ContainerBase::Render(RendererT& o_renderStream) const
{
    for (utils::intrusive_list_tag& listTag : *m_uiComponentsList)
    {
        static_cast<ComponentListTag&>(listTag).component->Render(o_renderStream);
    }
}

void ContainerBase::GetBegin(ComponentListTag*& o_listTag) const
{
    o_listTag = &static_cast<ComponentListTag&>(*m_uiComponentsList->begin());
}

void ContainerBase::GetFirstInteractive(InteractiveComponentTag*& o_listTag) const
{
    o_listTag = &static_cast<InteractiveComponentTag&>(*m_interactiveComponentsList->begin());
}

bool ContainerBase::IsEnd(ComponentListTag* i_listTag) const
{
    return i_listTag == m_endTag.get();
}

bool ContainerBase::IsEnd(InteractiveComponentTag* i_listTag) const
{
    return i_listTag == &m_endTag->interactiveTag;
}

void ContainerBase::Next(ComponentListTag*& io_listTag) const
{
    io_listTag = static_cast<ComponentListTag*>(io_listTag->next);
}

void ContainerBase::Next(InteractiveComponentTag*& io_listTag) const
{
    io_listTag = static_cast<InteractiveComponentTag*>(io_listTag->next);
}

void ContainerBase::Prev(InteractiveComponentTag*& io_listTag) const
{
    io_listTag = static_cast<InteractiveComponentTag*>(io_listTag->prev);
}

void ContainerBase::Prev(ComponentListTag*& io_listTag) const
{
    io_listTag = static_cast<ComponentListTag*>(io_listTag->prev);
}

const size_t& ContainerBase::GetComponentCount() const
{
    return m_componentCount;
}

IUIComponent* ContainerBase::GetUIComponent(ComponentListTag* i_listTag) const
{
    return i_listTag->component.get();
}

IUIComponent* ContainerBase::GetUIComponent(InteractiveComponentTag* i_listTag) const
{
    return i_listTag->componentTag->component.get();
}

IInputRelay* ContainerBase::GetInputRelay(ComponentListTag* i_listTag) const
{
    return i_listTag->inputRelay;
}

IInputRelay* ContainerBase::GetInputRelay(InteractiveComponentTag* i_listTag) const
{
    return i_listTag->componentTag->inputRelay;
}

utils::unique_ref<IUIComponent> ContainerBase::RetrieveUIComponent(ComponentListTag* i_listTag)
{
    utils::unique_ref<IUIComponent> retrievedComponent = std::move(i_listTag->component);
    if (i_listTag->inputRelay)
    {
        m_interactiveComponentsList->remove(i_listTag->interactiveTag);
    }
    delete &m_uiComponentsList->remove(*i_listTag);
    --m_componentCount;
    return retrievedComponent;
}
