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
        GAMEOVER
    };

    void Start();
    void Update();
    void End();

    void SetState(State state);
    State GetCurrentState() const { return m_CurrentState; }
    int GetCurrentLevel() const { return m_CurrentLevel; }
    std::shared_ptr<Nick> GetNick() const { return m_Nick; } // New method to access Nick

private:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    struct LevelConfig {
        int levelId;
        bool isBossLevel;
        std::vector<std::pair<std::string, glm::vec2>> enemies;
    };

    void InitializeLevel(int levelId);
    void UpdateFadeAnimation(float deltaTime);
    void SpawnEnemiesForLevel(int levelId);

    State m_CurrentState = State::START;
    int m_CurrentLevel = -1;

    Util::Renderer m_Root;
    std::shared_ptr<Nick> m_Nick;
    std::shared_ptr<PhaseResourceManger> m_PRM;
    std::shared_ptr<BackgroundImage> m_Overlay;

    float m_FadeTimer = 0.0f;
    const float m_FadeDuration = 4.0f;
    bool m_FadingIn = true;
    float m_GameOverTimer = 0.0f;
    const float m_GameOverDuration = 0.1f;

    std::vector<LevelConfig> m_LevelConfigs;
};

#endif