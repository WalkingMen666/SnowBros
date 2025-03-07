#include "App.hpp"

#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");

    m_test = std::make_shared<Character>(RESOURCE_DIR"/Image/Character/Boss/blue_pig.png");
    m_test->SetPosition({-112.5f, -140.5f});
    m_test->SetZIndex(50);
    m_Root.AddChild(m_test);



    m_PRM = std::make_shared<PhaseResourceManger>();
    m_Root.AddChildren(m_PRM->GetChildren());

    m_CurrentState = State::UPDATE;
}