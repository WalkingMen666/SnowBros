#include "App.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");
    m_PRM = std::make_shared<PhaseResourceManger>();
    m_Root.AddChildren(m_PRM->GetChildren());

    // 初始化覆蓋層為 BackgroundImage
    m_Overlay = std::make_shared<BackgroundImage>();
    m_Overlay->SetImage(RESOURCE_DIR "/Image/Background/black/black255.png");
    m_Overlay->m_Transform.scale = {820.0f, 720.0f}; // 直接設置縮放
    m_Overlay->SetZIndex(5); // 設置 Z 軸順序
    m_Root.AddChild(m_Overlay);

    m_Phase = Phase::Phase_1; // 從 Phase-1 開始
    m_FadeTimer = 0.0f;
    m_FadingIn = true;
    m_CurrentState = State::UPDATE;
}