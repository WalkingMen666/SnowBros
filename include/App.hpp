#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Util/Renderer.hpp"
#include "PhaseResourceManger.hpp"
#include "Util/Color.hpp"
#include "BackgroundImage.hpp"
#include "Util/Logger.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"
#include <memory>
#include <vector>
#include "UpdatableDrawable.hpp"

class Nick;
class Enemy;

enum class Direction { Left, Right };

class App {
public:
    enum class State { START, UPDATE, END, GAMEOVER, FINISH };

    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

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

    void Start();
    void Update();
    void End();
    void SetState(State state);
    void AddPendingObject(const std::shared_ptr<UpdatableDrawable>& obj) {
        m_PendingObjects.push_back(obj);
    }
    void AddRemovingObject(const std::shared_ptr<UpdatableDrawable>& obj) {
        m_RemovingObjects.push_back(obj);
    }

    [[nodiscard]] State GetCurrentState() const { return m_CurrentState; }
    [[nodiscard]] int GetCurrentLevel() const { return m_CurrentLevel; }
    [[nodiscard]] std::shared_ptr<Nick> GetNick() const { return m_Nick; }

private:
    // State and Level Management
    struct LevelConfig {
        int levelId;
        bool isBossLevel;
        std::vector<std::pair<std::string, glm::vec2>> enemies;
    };
    State m_CurrentState = State::START;
    int   m_CurrentLevel = -1;
    std::vector<LevelConfig> m_LevelConfigs;

    // Core Components
    Util::Renderer m_Root;
    std::shared_ptr<Nick> m_Nick;
    std::shared_ptr<PhaseResourceManger> m_PRM;
    std::shared_ptr<BackgroundImage> m_Overlay;

    // Background Music
    std::shared_ptr<Util::BGM> m_IntroBGM;
    std::shared_ptr<Util::BGM> m_Stage1BGM;
    std::shared_ptr<Util::BGM> m_Stage2BGM;
    std::shared_ptr<Util::BGM> m_BossBGM;
    std::shared_ptr<Util::SFX> m_GameOverBGM;
    void PlayBGMForLevel(int levelId);

    // Object Management
    std::vector<std::shared_ptr<UpdatableDrawable>> m_PendingObjects;
    std::vector<std::shared_ptr<UpdatableDrawable>> m_RemovingObjects;

    // UI
    std::shared_ptr<BackgroundImage> m_nickLives;
    // score
    std::vector<std::shared_ptr<BackgroundImage>> m_ScoreDigits; // 6 個數字物件
    std::vector<std::shared_ptr<Util::Image>> m_DigitImages; // 快取 0-9 圖片
    static constexpr int MAX_SCORE_DIGITS = 6; // 最大 6 位數
    float m_ScoreDigitSpacing = 40.0f; // 數字間距
    glm::vec2 m_ScorePosition = {320.0f, 360.0f}; // 右上角位置

    // Animation and Timing
    float m_FadeTimer = 0.0f;
    const float m_FadeDuration = 4.0f;
    bool  m_FadingIn = true;
    float m_GameOverTimer = 0.0f;
    const float m_GameOverDuration = 0.1f;
    float m_LevelingTimer = 0.0f;
    const float m_LevelingDuration = 1.5f;
    float m_EndTimer = 0.0f;
    const float m_EndDuration = 3.0f;

    // Private Methods
    void InitializeLevel(int levelId);
    void UpdateFadeAnimation(float deltaTime);
    void SpawnEnemiesForLevel(int levelId);
    void UpdateUI();
};

#endif // APP_HPP