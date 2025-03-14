#include "App.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    // 控制 Phase_1 的淡入淡出動畫
    if (m_Phase == Phase::Phase_1) {
        m_FadeTimer += deltaTime;
        float alpha = 0.0f;

        if (m_FadingIn) {
            // 從全黑 (255) 到全透明 (0)
            alpha = std::max(0.0f, 180.0f - (255.0f * (m_FadeTimer / (m_FadeDuration / 2))));
            if (m_FadeTimer >= m_FadeDuration / 2) {
                m_FadingIn = false;
                m_FadeTimer = 0.0f;
            }
        } else {
            // 從全透明 (0) 到全黑 (255)
            alpha = std::min(180.0f, 200.0f * (m_FadeTimer / (m_FadeDuration / 2)));
            if (m_FadeTimer >= m_FadeDuration / 2) {
                m_FadingIn = true;
                m_FadeTimer = 0.0f;
            }
        }

        // 更新覆蓋層圖片
        int alphaInt = static_cast<int>(alpha);
        std::string overlayPath = RESOURCE_DIR "/Image/Background/black/black" + std::to_string(alphaInt) + ".png";
        m_Overlay->SetImage(overlayPath);
        m_Overlay->SetVisible(true); // 確保覆蓋層可見

        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_Phase = Phase::Phase0;
            m_PRM->NextPhase(); // 切換到 m_Phase = 0 (phase0.map)
            m_FadeTimer = 0.0f;
            m_FadingIn = true;
            m_Overlay->SetVisible(false); // 隱藏覆蓋層
            LOG_DEBUG("Entered Phase 0");
        }
    }

    // Phase_0 選單畫面
    else if (m_Phase == Phase::Phase0) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_Phase = Phase::Phase1;
            m_PRM->NextPhase(); // 切換到 m_Phase = 1 (phase1.map)
            m_Nick = std::make_shared<Nick>(); // Nick 在 Phase_1 生成
            m_Root.AddChild(m_Nick);
            m_Nick->SetState(Nick::State::SPAWN);
            LOG_DEBUG("Entered Phase 1, Nick initialized");
        }
    }

    // Phase_1 遊戲進
    if (m_Phase == Phase::Phase1) {
        glm::vec2 nickPosition = m_Nick->GetPosition();
        if (nickPosition.x >= 800.0f) {
            m_Phase = Phase::Phase2;
            m_PRM->NextPhase(); // 切換到 m_Phase = 0 (phase0.map)
            m_Nick->SetPosition({0.0f, -270.0f});
            LOG_DEBUG("Entered Phase {}", m_PRM->GetPhase());
        }
    }

    if (m_Nick) {
        m_Nick->Update();
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}