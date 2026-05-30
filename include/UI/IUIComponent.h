#pragma once
#include "AsyncScopedHelper.h"
#include "UIContext.h"
#include "UI/IRenderStyle.h"
#include "Components/IComponent.h"

class IUIComponent : virtual public IComponent
{
public:
	IUIComponent() = delete;
	IUIComponent(const UIContext& i_uiContext) : m_uiContext(i_uiContext) {}
	virtual void SetRenderStyle(utils::unique_ref<IRenderStyle> i_renderStyle) { m_renderStyle = std::move(i_renderStyle); m_renderStyle->OnThisRenderStyleApplied(); }
	IRenderStyle* GetRenderStyle() const { return m_renderStyle.get(); }
	virtual utils::unique_ptr<IRenderStyle> RetrieveRenderStyle() { if (m_renderStyle) { m_renderStyle->OnThisRenderStyleUnapplied(); } return std::move(m_renderStyle); }
	virtual void ResetRenderStyle() { if (m_renderStyle) { m_renderStyle->OnThisRenderStyleUnapplied(); } m_renderStyle.reset(); }
	virtual const UIContext& GetUIContext() const final { return m_uiContext; }
	virtual uint16_t GetWidth() const { return 0u; }
	virtual uint16_t GetHeight() const { return 0u; }
	virtual void SetWidth(const uint16_t&) {}
	virtual void SetHeight(const uint16_t&) {}
	virtual void OnShow() const {}
	virtual void OnHide() const {}
	virtual void OnFocusGained(const utils::RGBColor& i_focusColor = utils::RGBColor(0, 255, 0)) {}
	virtual void OnFocusLost() {}
	virtual void Render(RendererT& o_renderStream) const = 0;
	template <typename... Args>
	void StartOptionalTask(Args&&... args)
	{
		m_asyncScopedHelper.StartOptionalTask(std::forward<Args>(args)...);
	}

protected:
	UIContext m_uiContext;
	utils::unique_ptr<IRenderStyle> m_renderStyle;
	utils::AsyncScopedHelper m_asyncScopedHelper;
};