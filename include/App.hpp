#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Util/Renderer.hpp"
#include "Nick.hpp"
#include "PhaseResourceManger.hpp"

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End();

private:
    enum class Phase {
        Phase0,  // 起始頁
        Phase1,
        Phase2,
    };

    State m_CurrentState = State::START;
    Phase m_Phase = Phase::Phase0;

    Util::Renderer m_Root;
    std::shared_ptr<Nick> m_Nick; // 在 Phase 1 時初始化
    std::shared_ptr<PhaseResourceManger> m_PRM;
};

#endif