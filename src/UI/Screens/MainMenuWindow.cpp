#include "stdafx.h"
#include "UI/Screens/MainMenuWindow.h"
#include "Components/ButtonComponent.h"
#include "Components/InputtableSetting.h"
#include "Components/InputExchangerTemplate.h"
#include "Components/SelectableSetting.h"
#include "Components/OptionTemplate.h"
#include "DisplayInfo.h"
#include "Gameplay/Logic/LogicConfig.h"
#include "UI/IUIManager.h"

#include <cctype>

namespace
{
constexpr uint16_t k_componentHeight = 3;

bool my_isdigit(const char& ch)
{
    return std::isdigit(static_cast<unsigned char>(ch));
}
}

MainMenuWindow::MainMenuWindow(const UIContext& i_uiContext, LogicConfig& i_logicConfig)
    : IUIComponent(i_uiContext)
    , WindowBase(i_uiContext)
    , m_logicConfig(i_logicConfig)
{
    uint16_t width = m_uiContext.uiManager.GetDisplayInfo().width;
    MakeAndMapUIComponent<ButtonComponent>(m_errorInfoButton);
    MakeAndMapUIComponent<InputtableSetting>(m_initialCoinsSetting, "Initial Coins", width, k_componentHeight);
    MakeAndMapUIComponent<InputtableSetting>(m_initialHitRateSetting, "Initial Hit Rate", width, k_componentHeight);
    MakeAndMapUIComponent<InputtableSetting>(m_defaultHitRateSetting, "Default Hit Rate", width, k_componentHeight);
    MakeAndMapUIComponent<InputtableSetting>(m_hitRateStepSetting, "Hit Rate Step", width, k_componentHeight);
    MakeAndMapUIComponent<InputtableSetting>(m_cardCountSetting, "Number Of Cards", width, k_componentHeight);
    MakeAndMapUIComponent<SelectableSetting>(m_difficultySetting, "Difficulty", width, k_componentHeight);
    MakeAndMapUIComponent<ButtonComponent>(m_startGameButton, "Start Game", width, k_componentHeight);
    MakeAndMapUIComponent<ButtonComponent>(m_exitGameButton, "Exit Game", width, k_componentHeight);
    Initialize();
}

utils::unique_ref<IComponent> MainMenuWindow::Clone()
{
    auto clone = utils::make_unique<MainMenuWindow>(GetUIContext(), m_logicConfig);
    static_cast<WindowBase&>(*clone) = *this;
    return clone;
}

void MainMenuWindow::Initialize()
{
    m_allSettings = { m_initialCoinsSetting, m_initialHitRateSetting, m_defaultHitRateSetting, m_hitRateStepSetting, m_cardCountSetting };
    for (IInputtableSetting* setting : m_allSettings)
    {
        m_signalConnections.push_back(setting->sig_onValueChanged.Connect(&MainMenuWindow::OnValueChanged, this, setting));
        m_signalConnections.push_back(setting->sig_onErrorEncountered.Connect(&MainMenuWindow::OnInputErrorEncountered, this, setting));
    }
    m_initialCoinsSetting->SetInputExchanger(utils::make_unique<InputExchangerTemplate<uint32_t>>(m_logicConfig.defaultBid, std::nullopt, [](const std::string& i_input) -> uint32_t { return i_input.empty() ? 0 : std::stoul(i_input); }, my_isdigit, InputParserT<uint32_t>(utils::ConvertToString<char, uint32_t>)));
    m_initialHitRateSetting->SetInputExchanger(utils::make_unique<InputExchangerTemplate<float>>(m_logicConfig.normBidHitRate, std::nullopt, [](const std::string& i_input) -> float { return i_input.empty() ? 0.0f : std::stof(i_input); }, [](const char& i_char) -> bool { return my_isdigit(i_char) || i_char == '.'; }, InputParserT<float>(utils::ConvertToString<char, float>)));
    m_defaultHitRateSetting->SetInputExchanger(utils::make_unique<InputExchangerTemplate<float>>(m_logicConfig.defaultHitRate, std::nullopt, [](const std::string& i_input) -> float { return i_input.empty() ? 0.0f : std::stof(i_input); }, [](const char& i_char) -> bool { return my_isdigit(i_char) || i_char == '.'; }, InputParserT<float>(utils::ConvertToString<char, float>)));
    m_hitRateStepSetting->SetInputExchanger(utils::make_unique<InputExchangerTemplate<float>>(m_logicConfig.hitRateStep, std::nullopt, [](const std::string& i_input) -> float { return i_input.empty() ? 0.0f : std::stof(i_input); }, [](const char& i_char) -> bool { return my_isdigit(i_char) || i_char == '.'; }, InputParserT<float>(utils::ConvertToString<char, float>)));
    m_cardCountSetting->SetInputExchanger(utils::make_unique<InputExchangerTemplate<uint16_t>>(m_logicConfig.cardCount, std::nullopt, [](const std::string& i_input) -> uint16_t { return i_input.empty() ? 0 : static_cast<uint16_t>(std::stoul(i_input)); }, my_isdigit, InputParserT<uint16_t>(utils::ConvertToString<char, uint16_t>)));
    m_difficultySetting->AddOptions({ utils::make_unique<OptionTemplate<LogicConfig::Difficulty>>(LogicConfig::Difficulty::Hard), utils::make_unique<OptionTemplate<LogicConfig::Difficulty>>(LogicConfig::Difficulty::Medium), utils::make_unique<OptionTemplate<LogicConfig::Difficulty>>(LogicConfig::Difficulty::Easy) });
    m_signalConnections.push_back(m_startGameButton->sig_onAction.Connect(&MainMenuWindow::OnStartGameButtonPressed, this));
    m_signalConnections.push_back(m_exitGameButton->sig_onAction.Connect(&MainMenuWindow::OnExitGameButtonPressed, this));
    m_signalConnections.push_back(m_errorInfoButton->sig_onAction.Connect(&MainMenuWindow::OnErrorInfoButtonPressed, this));
}

void MainMenuWindow::OnStartGameButtonPressed()
{
    for (IInputtableSetting* setting : m_allSettings)
    {
        HideErrorInfo(setting);
        setting->GetInputExchanger()->RequestExchange();
    }
    if (m_isAnyErrorEncountered)
    {
        return;
    }
    Close(StartGameActionTag{});
}

void MainMenuWindow::OnExitGameButtonPressed()
{
    Close(ExitGameActionTag{});
}

void MainMenuWindow::OnValueChanged(IInputtableSetting* i_setting)
{
    HideErrorInfo(i_setting);
}

void MainMenuWindow::OnDifficultyOptionChanged(IOption& i_option)
{
   m_logicConfig.difficulty = static_cast<OptionTemplate<LogicConfig::Difficulty>&>(i_option).GetValue();
}

void MainMenuWindow::OnErrorInfoButtonPressed()
{
    HideErrorInfo();
}

void MainMenuWindow::OnInputErrorEncountered(IInputtableSetting* i_setting, InputErrorType i_errorType)
{
    std::string errorMessage = "Error in setting '" + i_setting->GetText() + "': ";
    switch (i_errorType)
    {
    case InputErrorType::InvalidInput:
        errorMessage += "Invalid input.";
        break;
    case InputErrorType::ExchangeFailed:
        errorMessage += "Failed to apply the input.";
        break;
    case InputErrorType::TextUpdateFailed:
        errorMessage += "Failed to update the text input.";
        break;
    }
    ShowErrorInfo(i_setting, errorMessage);
}

void MainMenuWindow::ShowErrorInfo(IInputtableSetting* i_setting, const std::string& i_errorMessage)
{
    m_isAnyErrorEncountered |= m_settingErrorStatus[i_setting] = true;
    m_errorInfoButton->SetText(i_errorMessage);
    IUIComponent& component = dynamic_cast<IUIComponent&>(*m_errorInfoButton);
    component.SetWidth(m_uiContext.uiManager.GetDisplayInfo().width);
    component.SetHeight(k_componentHeight);
    SetActiveComponent(dynamic_cast<IUIComponent&>(*i_setting), utils::RGBColor(255, 0, 0));
}

void MainMenuWindow::HideErrorInfo(IInputtableSetting* i_setting)
{
    if (i_setting != nullptr)
    {
        m_isAnyErrorEncountered = false;
        m_settingErrorStatus[i_setting] = false;
        for (const auto& [setting, hasError] : m_settingErrorStatus)
        {
            m_isAnyErrorEncountered |= hasError;
        }
    }
    if (m_isAnyErrorEncountered)
    {
        return;
    }
    m_errorInfoButton->SetText("");
    IUIComponent& component = dynamic_cast<IUIComponent&>(*m_errorInfoButton);
    component.SetWidth(0);
    component.SetHeight(0);
}