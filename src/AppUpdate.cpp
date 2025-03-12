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
            m_Nick->SetState(Nick::State::SPAWN);
            LOG_DEBUG("Entered Phase 1, Nick initialized");
        }
    }

    if (m_Phase == Phase::Phase1) {
        glm::vec2 nickPosition = m_Nick->GetPosition();
        if (nickPosition.x >= 800.0f) {
            m_Phase = Phase::Phase2;
            m_PRM->NextPhase();
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
