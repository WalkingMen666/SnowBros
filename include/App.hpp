#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Util/Renderer.hpp"
#include "PhaseResourceManger.hpp"
#include "Util/Color.hpp"
#include "BackgroundImage.hpp"
#include "Util/Logger.hpp"
#include <memory>
#include <vector>

class Nick;
class Enemy;

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

    static Util::Renderer& GetRoot() {
        static App* instance = &GetInstance();
        return instance->m_Root;
    }

    enum class State {
        START,
        UPDATE,
        END,
    };

    void Start();
    void Update();
    void End();

    State GetCurrentState() const { return m_CurrentState; }
    int GetCurrentLevel() const { return m_CurrentLevel; }

private:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    // 關卡配置結構
    struct LevelConfig {
        int levelId;
        bool isBossLevel;
        std::vector<std::pair<std::string, glm::vec2>> enemies; // 敵人類型與生成位置
    };

    void InitializeLevel(int levelId);      // 初始化關卡
    void UpdateFadeAnimation(float deltaTime); // 更新淡入淡出動畫
    void SpawnEnemiesForLevel(int levelId); // 生成關卡敵人

    State m_CurrentState = State::START;
    int m_CurrentLevel = -1; // -1: 初始畫面, 0: 開始畫面, 1~30: 關卡

    Util::Renderer m_Root;
    std::shared_ptr<Nick> m_Nick;
    std::shared_ptr<PhaseResourceManger> m_PRM;
    std::shared_ptr<BackgroundImage> m_Overlay;

    // 動畫相關
    float m_FadeTimer = 0.0f;
    const float m_FadeDuration = 4.0f;
    bool m_FadingIn = true;

    // 關卡配置數據
    std::vector<LevelConfig> m_LevelConfigs;
};

#endif