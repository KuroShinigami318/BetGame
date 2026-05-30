#pragma once

#include "UI/IUIComponent.h"
#include "Control/IInputRelay.h"

enum class IContainerErrorCode : uint8_t;

class IContainer : virtual public IUIComponent
{
public:
    using IContainerError = utils::Error<IContainerErrorCode>;
	using RetrieveResult = utils::Result<utils::unique_ref<IUIComponent>, IContainerError>;

public:
    virtual IUIComponent& AddUIComponent(utils::unique_ref<IUIComponent> i_uiComponent) = 0;
    virtual RetrieveResult RetrieveUIComponent(IUIComponent& i_uiComponent) = 0;
    template <typename TUIComponent, typename T, typename ... Args>
    void MakeAndMapUIComponent(T*& o_component, Args&&... args)
    {
        static_assert(std::is_base_of_v<IUIComponent, TUIComponent>, "TUIComponent must be derived from IUIComponent");
        auto uiComponent = utils::make_unique<TUIComponent>(GetUIContext(), std::forward<Args>(args)...);
        o_component = uiComponent.get();
        AddUIComponent(std::move(uiComponent));
    }
};