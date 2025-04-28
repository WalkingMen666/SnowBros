#include "App.hpp"
#include "Util/Logger.hpp"

App::App() {
    // 初始化關卡配置（確保正確）
    m_LevelConfigs = {
        { -1, false, {} }, // 初始畫面
        { 0, false, {} },  // 開始畫面
        { 1, false, { {"RedDemon", {-260.0f, 160.0f}}, {"RedDemon", {260.0f, 160.0f}}, {"Frog", {0.0f, -40.0f}},{"SmallBoss",{650.0f,130.0f}} } },
        { 2, false, { {"RedDemon", {150.0f, 100.0f}}, {"RedDemon", {-150.0f, 100.0f}}, {"RedDemon", {-110.0f, -185.0f}}, {"RedDemon", {65.0f, -80.0f}}}},
        { 3, false, {} }, // 第3關: 無敵人
        { 10,true, { {"Boss", {300.0f, 92.0f}}, } }, // 第10關: 1隻 Boss
        // { 28, true, { {"Boss1", {300.0f, 200.0f}} } }, // Boss 關示例
        // { 29, true, { {"Boss2", {350.0f, 250.0f}} } },
        // { 30, true, { {"Boss3", {400.0f, 300.0f}} } },
    };
}

void App::Start() {
    LOG_TRACE("Start");
    m_PRM = std::make_shared<PhaseResourceManger>();
    m_Root.AddChildren(m_PRM->GetChildren());

    // 初始化覆蓋層
    m_Overlay = std::make_shared<BackgroundImage>();
    m_Overlay->SetImage(RESOURCE_DIR "/Image/Background/black/black255.png");
    m_Overlay->m_Transform.scale = {1.0f, 1.0f}; // 設置縮放
    m_Overlay->SetZIndex(5); // 設置 Z 軸順序
    m_Root.AddChild(m_Overlay);

    // 初始化 UI
    m_nickLives = std::make_shared<BackgroundImage>();
    m_nickLives->SetImage(RESOURCE_DIR "/Image/Text/red_3.png");
    m_nickLives->m_Transform.translation = {-320.0f, 350.0f};
    m_nickLives->SetZIndex(5);
    m_nickLives->SetVisible(false);
    m_Root.AddChild(m_nickLives);

    // 初始化為初始畫面
    InitializeLevel(-1);
    m_CurrentState = State::UPDATE;
    LOG_INFO("App started with initial level -1");
}