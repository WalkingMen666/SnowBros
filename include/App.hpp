#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Util/Renderer.hpp"
#include "Nick.hpp"
#include "PhaseResourceManger.hpp"
#include "Util/Color.hpp"
#include "BackgroundImage.hpp" // 包含 BackgroundImage

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
        Phase_1, // 初始暗到亮再到暗的畫面，對應 m_Phase = 1 (phase1.map)
        Phase0,  // 選單畫面，對應 m_Phase = 0 (phase0.map)
        Phase1,  // 遊戲開始，生成 Nick，對應 m_Phase = 1 (phase1.map)
        Phase2,  // 第二階段，對應 m_Phase = 0 (phase0.map)
    };

    State m_CurrentState = State::START;
    Phase m_Phase = Phase::Phase_1;

    Util::Renderer m_Root;          // 管理渲染物件
    std::shared_ptr<Nick> m_Nick;
    std::shared_ptr<PhaseResourceManger> m_PRM;
    std::shared_ptr<BackgroundImage> m_Overlay; // 改為 BackgroundImage

    // 動畫相關
    float m_FadeTimer = 0.0f;
    const float m_FadeDuration = 4.0f; // 每段淡入/淡出 3 秒，總共 6 秒
    bool m_FadingIn = true; // 控制淡入還是淡出
};

#endif