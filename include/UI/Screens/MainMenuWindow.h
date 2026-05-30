#pragma once

#include "UI/WindowBase.h"

class IButton;
struct IInputExchanger;
class IInputtableSetting;
class ISelectableSetting;
class IOption;
struct LogicConfig;
struct StartGameActionTag {};
struct ExitGameActionTag {};

enum class InputErrorType;

class MainMenuWindow : public WindowBase
{
public:
    MainMenuWindow(const UIContext& i_uiContext, LogicConfig& i_logicConfig);
    utils::unique_ref<IComponent> Clone() override;

private:
    void Initialize();
    void OnStartGameButtonPressed();
    void OnExitGameButtonPressed();
    void OnValueChanged(IInputtableSetting* i_setting);
    void OnDifficultyOptionChanged(IOption& i_option);
    void OnErrorInfoButtonPressed();
    void OnInputErrorEncountered(IInputtableSetting* i_setting, InputErrorType i_errorType);
    void ShowErrorInfo(IInputtableSetting* i_setting, const std::string& i_errorMessage);
    void HideErrorInfo(IInputtableSetting* i_setting = nullptr);

private:
    LogicConfig& m_logicConfig;
    bool m_isAnyErrorEncountered = false;
    IInputtableSetting* m_initialCoinsSetting;
    IInputtableSetting* m_initialHitRateSetting;
    IInputtableSetting* m_defaultHitRateSetting;
    IInputtableSetting* m_hitRateStepSetting;
    IInputtableSetting* m_cardCountSetting;
    ISelectableSetting* m_difficultySetting;
    IButton* m_startGameButton;
    IButton* m_exitGameButton;
    IButton* m_errorInfoButton;
    std::unordered_map<IInputtableSetting*, bool> m_settingErrorStatus;
    std::vector<IInputtableSetting*> m_allSettings;
    std::vector<utils::Connection> m_signalConnections;
};