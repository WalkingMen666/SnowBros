#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Update() {
    if(m_Phase == Phase::Start_Page) {
        if(Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_Phase = Phase::Phase1;
            m_PRM->NextPhase();
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

    m_Root.Update();
}