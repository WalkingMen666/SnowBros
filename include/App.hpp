#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Util/Renderer.hpp"
#include "Nick.hpp"
#include "PhaseResourceManger.hpp"

class App {
public:
    static App& GetInstance() {
        static App instance;
        return instance;
    }

    static PhaseResourceManger* GetPRM() {
        auto& instance = GetInstance();
        if (!instance.m_PRM) {
            LOG_ERROR("PhaseResourceManger is not initialized!");
            return nullptr;
        }
        return instance.m_PRM.get();
    }

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
    App() = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    enum class Phase {
        Phase0,
        Phase1,
        Phase2,
    };

    State m_CurrentState = State::START;
    Phase m_Phase = Phase::Phase0;

    Util::Renderer m_Root;          //管理渲染物件
    std::shared_ptr<Nick> m_Nick;
    std::shared_ptr<PhaseResourceManger> m_PRM;
};

#endif