#include "App.hpp"
#include "Util/Logger.hpp"

App::App() {
    // 初始化關卡配置（確保正確）
    m_LevelConfigs = {
        { -1, false, {} }, // 初始畫面
        { 0, false, {} },  // 開始畫面
        { 1, false, { {"RedDemon", {260.0f, 180.0f}} } }, // 第1關: 1隻 RedDemon
        { 2, false, { {"RedDemon", {200.0f, 100.0f}}, {"RedDemon", {-200.0f, 100.0f}} } }, // 第2關: 2隻 RedDemon
        { 3, false, {} }, // 第3關: 無敵人
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
    m_Overlay->m_Transform.scale = {820.0f, 720.0f}; // 設置縮放
    m_Overlay->SetZIndex(5); // 設置 Z 軸順序
    m_Root.AddChild(m_Overlay);

    // 初始化為初始畫面
    InitializeLevel(-1);
    m_CurrentState = State::UPDATE;
    LOG_INFO("App started with initial level -1");
}