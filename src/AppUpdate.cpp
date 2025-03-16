#include "App.hpp"
#include "Nick.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameWorld.hpp"
#include "RedDemon.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"

void App::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    // 更新淡入淡出動畫
    UpdateFadeAnimation(deltaTime);

    // 同步 PhaseResourceManger 的 Phase 與 App 的 Level
    if (m_PRM && m_PRM->GetPhase() != m_CurrentLevel) {
        m_CurrentLevel = m_PRM->GetPhase();
        LOG_DEBUG("Syncing m_CurrentLevel to Phase: {}", m_CurrentLevel);
    }

    // 檢查並切換關卡
    if (m_Nick) {
        glm::vec2 nickPosition = m_Nick->GetPosition();
        glm::vec2 newPosition = nickPosition; // 預設返回當前位置
        static bool hasSwitchedPhase = false; // 防止連續跳關

        if (Util::Input::IsKeyDown(Util::Keycode::N) && !hasSwitchedPhase) {
            m_PRM->NextPhase(); // 切換到下一關
            m_CurrentLevel = m_PRM->GetPhase(); // 立即同步
            newPosition = {0.0f, -285.0f}; // 重置位置
            hasSwitchedPhase = true;
            LOG_INFO("Entering Phase/Level: {}", m_CurrentLevel);

            // 移除所有 GameWorld 物件並從渲染樹中移除
            for (auto obj : GameWorld::GetObjects()) {
                m_Root.RemoveChild(obj);
            }
            GameWorld::GetObjects().clear(); // 清除前一關的所有物件

            // 移除舊的 Nick 並重新生成
            m_Root.RemoveChild(m_Nick);
            m_Nick = std::make_shared<Nick>();
            m_Nick->SetPosition(newPosition);
            m_Nick->SetState(Nick::State::SPAWN);
            m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(m_Nick));

            // 生成新關卡的敵人
            SpawnEnemiesForLevel(m_CurrentLevel);
        } else if (!Util::Input::IsKeyDown(Util::Keycode::N)) {
            hasSwitchedPhase = false; // 重置切換標記
        }
    }

    // 關卡邏輯
    if (m_CurrentLevel == -1) { // 初始畫面 (Phase-1)
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_PRM->NextPhase(); // 從 Phase-1 切換到 Phase0
            InitializeLevel(0); // 初始化 Level 0
            LOG_INFO("Entered Level 0 (Start Screen)");
        }
    } else if (m_CurrentLevel == 0) { // 開始畫面 (Phase0)
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_PRM->NextPhase(); // 從 Phase0 切換到 Phase1
            InitializeLevel(1); // 初始化 Level 1
            LOG_INFO("Entered Level 1, Nick and RedDemon initialized");
        }
    } else if (m_CurrentLevel >= 1 && m_CurrentLevel <= 30) { // 遊戲關卡
        auto& objects = GameWorld::GetObjects();
        std::vector<std::shared_ptr<UpdatableDrawable>> toRemove;

        // 更新所有物件
        for (auto& obj : objects) {
            obj->Update();
            if (auto bullet = std::dynamic_pointer_cast<Bullet>(obj)) {
                if (bullet->IsMarkedForRemoval()) {
                    toRemove.push_back(obj);
                }
            } else if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                if (enemy->GetState() == EnemyState::Dead &&
                    std::dynamic_pointer_cast<Util::Animation>(enemy->GetDrawable())->GetState() == Util::Animation::State::ENDED) {
                    toRemove.push_back(obj);
                }
            }
        }

        // 移除標記為刪除的物件
        for (const auto& obj : toRemove) {
            GameWorld::RemoveObject(obj);
            m_Root.RemoveChild(obj);
        }

        // 更新主角
        if (m_Nick) {
            m_Nick->Update();
        }

        // 檢查是否進入下一關
        if (GameWorld::GetObjects().empty() && m_Nick && m_Nick->GetState() != Nick::State::DIE) {
            if (m_CurrentLevel < 30) {
                // 移除所有 GameWorld 物件並從渲染樹中移除
                for (auto obj : GameWorld::GetObjects()) {
                    m_Root.RemoveChild(obj);
                }
                GameWorld::GetObjects().clear();

                m_PRM->NextPhase();
                InitializeLevel(m_PRM->GetPhase());
                LOG_INFO("Entered Level {}", m_CurrentLevel);
            } else {
                m_CurrentState = State::END;
                LOG_INFO("Game Completed!");
            }
        }
    }

    // 退出遊戲
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}

void App::UpdateFadeAnimation(float deltaTime) {
    if (m_CurrentLevel != -1) return; // 僅初始畫面需要淡入淡出

    m_FadeTimer += deltaTime;
    float alpha = 0.0f;
    if (m_FadingIn) {
        alpha = std::max(0.0f, 180.0f - (255.0f * (m_FadeTimer / (m_FadeDuration / 2))));
        if (m_FadeTimer >= m_FadeDuration / 2) {
            m_FadingIn = false;
            m_FadeTimer = 0.0f;
        }
    } else {
        alpha = std::min(180.0f, 200.0f * (m_FadeTimer / (m_FadeDuration / 2)));
        if (m_FadeTimer >= m_FadeDuration / 2) {
            m_FadingIn = true;
            m_FadeTimer = 0.0f;
        }
    }
    int alphaInt = static_cast<int>(alpha);
    std::string overlayPath = RESOURCE_DIR "/Image/Background/black/black" + std::to_string(alphaInt) + ".png";
    m_Overlay->SetImage(overlayPath);
    m_Overlay->SetVisible(true);
}

void App::InitializeLevel(int levelId) {
    m_CurrentLevel = levelId;
    m_FadeTimer = 0.0f;
    m_FadingIn = true;
    m_Overlay->SetVisible(levelId == -1); // 僅初始畫面顯示淡入淡出

    if (levelId == 1) { // 第1關初始化主角和敵人
        if (!m_Nick) {
            m_Nick = std::make_shared<Nick>();
            m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(m_Nick));
            m_Nick->SetState(Nick::State::SPAWN);
        }
        // 移除所有 GameWorld 物件並從渲染樹中移除
        for (auto obj : GameWorld::GetObjects()) {
            m_Root.RemoveChild(obj);
        }
        GameWorld::GetObjects().clear();
        SpawnEnemiesForLevel(levelId); // 生成當前關卡敵人
        LOG_DEBUG("Initialized Level {} with {} enemies", levelId, GameWorld::GetObjects().size());
    } else if (levelId >= 2 && levelId <= 30) {
        // 移除所有 GameWorld 物件並從渲染樹中移除
        for (auto obj : GameWorld::GetObjects()) {
            m_Root.RemoveChild(obj);
        }
        GameWorld::GetObjects().clear();
        SpawnEnemiesForLevel(levelId); // 生成當前關卡敵人
        LOG_DEBUG("Initialized Level {} with {} enemies", levelId, GameWorld::GetObjects().size());
    }
}

void App::SpawnEnemiesForLevel(int levelId) {
    for (const auto& config : m_LevelConfigs) {
        if (config.levelId == levelId) {
            for (const auto& [enemyType, pos] : config.enemies) {
                std::shared_ptr<Enemy> enemy;
                if (enemyType == "RedDemon") {
                    enemy = std::make_shared<RedDemon>(pos);
                    LOG_DEBUG("Spawning RedDemon at {}", glm::to_string(pos));
                } else if (enemyType == "Boss1" || enemyType == "Boss2" || enemyType == "Boss3") {
                    enemy = std::make_shared<RedDemon>(pos); // 未來可替換為 Boss 類型
                    LOG_INFO("Spawning Boss {} at {}", enemyType, glm::to_string(pos));
                }
                if (enemy) {
                    GameWorld::AddObject(enemy);
                    m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(enemy));
                }
            }
            break;
        }
    }
}