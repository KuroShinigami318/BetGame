#include "stdafx.h"
#include "UI/Screens/SplashscreenWindow.h"
#include "Components/ProgressComponent.h"

SplashscreenWindow::SplashscreenWindow(const UIContext& i_uiContext, const float& i_barWidth)
	: IUIComponent(i_uiContext)
	, WindowBase(i_uiContext)
	, m_barWidth(i_barWidth)
{
	utils::unique_ref<ProgressComponent> progressComponent = utils::make_unique<ProgressComponent>(i_uiContext, i_barWidth);
	progressComponent->UpdateProgress(-1);
	AddUIComponent(std::move(progressComponent));
}

utils::unique_ref<IComponent> SplashscreenWindow::Clone()
{
	auto clone = utils::make_unique<SplashscreenWindow>(m_uiContext, m_barWidth);
	static_cast<WindowBase&>(*clone) = *this;
	return clone;
}
