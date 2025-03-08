#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Update() {
    if (m_Phase == Phase::Phase0) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_Phase = Phase::Phase1;
            m_PRM->NextPhase();
            m_Nick = std::make_shared<Nick>();
            m_Root.AddChild(m_Nick);
            m_Nick->SetState(Nick::State::SPAWN); // 確保觸發 Spawn
            LOG_DEBUG("Entered Phase 1, Nick initialized");
        }
    }


    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    if (m_Nick) {
        m_Nick->Update(); // 手動更新 Nick
    }
    m_Root.Update(); // 更新渲染樹
}