#pragma once

#include "IContainer.h"

namespace utils
{
class intrusive_list;
class intrusive_list_tag;
}

struct ComponentListTag;
struct InteractiveComponentTag;

class ContainerBase : virtual public IContainer
{
public:
    ~ContainerBase();
    ContainerBase(const UIContext& i_uiContext);
    ContainerBase(ContainerBase&&) noexcept;
    ContainerBase& operator=(ContainerBase&&) noexcept;
    ContainerBase& operator=(const ContainerBase&);
    IUIComponent& AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent) final;
    RetrieveResult RetrieveUIComponent(IUIComponent& i_uiComponent) override;
    void Render(RendererT& o_renderStream) const override;
    void GetBegin(ComponentListTag*& o_listTag) const;
    void GetFirstInteractive(InteractiveComponentTag*& o_listTag) const;
    bool IsEnd(ComponentListTag* i_listTag) const;
    bool IsEnd(InteractiveComponentTag* i_listTag) const;
    void Next(ComponentListTag*& io_listTag) const;
    void Next(InteractiveComponentTag*& io_listTag) const;
    void Prev(ComponentListTag*& io_listTag) const;
    void Prev(InteractiveComponentTag*& io_listTag) const;
    const size_t& GetComponentCount() const;
    IUIComponent* GetUIComponent(ComponentListTag* i_listTag) const;
    IUIComponent* GetUIComponent(InteractiveComponentTag* i_listTag) const;
    IInputRelay* GetInputRelay(ComponentListTag* i_listTag) const;
    IInputRelay* GetInputRelay(InteractiveComponentTag* i_listTag) const;
    utils::unique_ref<IUIComponent> RetrieveUIComponent(ComponentListTag* i_listTag);

private:
    utils::unique_ref<ComponentListTag> m_endTag;
    utils::unique_ref<utils::intrusive_list> m_uiComponentsList;
    utils::unique_ref<utils::intrusive_list> m_interactiveComponentsList;
    size_t m_componentCount;
};