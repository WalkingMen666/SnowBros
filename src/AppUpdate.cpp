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

    UpdateFadeAnimation(deltaTime);

    if (m_PRM && m_PRM->GetPhase() != m_CurrentLevel) {
        m_CurrentLevel = m_PRM->GetPhase();
        LOG_DEBUG("Syncing m_CurrentLevel to Phase: {}", m_CurrentLevel);
    }

    if (m_CurrentState == State::GAMEOVER) {
        m_GameOverTimer += deltaTime;
        LOG_DEBUG("GameOver timer: {}, duration: {}", m_GameOverTimer, m_GameOverDuration);

        static int lastFrame = -1;
        int currentFrame = static_cast<int>(m_GameOverTimer / m_GameOverDuration);
        if (currentFrame != lastFrame && currentFrame <= 3) {
            std::string path;
            if (currentFrame >= 3) {
                path = RESOURCE_DIR "/Image/Background/Game_Over_4.png";
            } else if (currentFrame >= 2) {
                path = RESOURCE_DIR "/Image/Background/Game_Over_3.png";
            } else if (currentFrame >= 1) {
                path = RESOURCE_DIR "/Image/Background/Game_Over_2.png";
            } else {
                path = RESOURCE_DIR "/Image/Background/Game_Over_1.png";
            }
            LOG_INFO("Setting overlay to: {}, frame: {}", path, currentFrame);
            m_Overlay->SetImage(path);
            m_Overlay->m_Transform.translation = {0.0f, 0.0f}; // 確保居中
            m_Overlay->m_Transform.scale = {720.0f, 720.0f};
            m_Overlay->SetZIndex(10000);
            m_Overlay->SetVisible(true);
            lastFrame = currentFrame;
        }
    }

    if (m_Nick) {
        glm::vec2 nickPosition = m_Nick->GetPosition();
        glm::vec2 newPosition = nickPosition;
        static bool hasSwitchedPhase = false;

        if (Util::Input::IsKeyDown(Util::Keycode::N) && !hasSwitchedPhase) {
            m_PRM->NextPhase();
            m_CurrentLevel = m_PRM->GetPhase();
            newPosition = {0.0f, -285.0f};
            hasSwitchedPhase = true;
            LOG_INFO("Entering Phase/Level: {}", m_CurrentLevel);

            for (auto obj : GameWorld::GetObjects()) {
                m_Root.RemoveChild(obj);
            }
            GameWorld::GetObjects().clear();

            m_Root.RemoveChild(m_Nick);
            m_Nick = std::make_shared<Nick>();
            m_Nick->SetPosition(newPosition);
            m_Nick->SetState(Nick::State::SPAWN);
            m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(m_Nick));

            SpawnEnemiesForLevel(m_CurrentLevel);
        } else if (!Util::Input::IsKeyDown(Util::Keycode::N)) {
            hasSwitchedPhase = false;
        }
    }

    if (m_CurrentLevel == -1) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_PRM->NextPhase();
            InitializeLevel(0);
            LOG_INFO("Entered Level 0 (Start Screen)");
        }
    } else if (m_CurrentLevel == 0) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_PRM->NextPhase();
            InitializeLevel(1);
            LOG_INFO("Entered Level 1, Nick and RedDemon initialized");
        }
    } else if (m_CurrentLevel >= 1 && m_CurrentLevel <= 30) {
        auto& objects = GameWorld::GetObjects();
        std::vector<std::shared_ptr<UpdatableDrawable>> toRemove;

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

        if (m_Nick) {
            m_Nick->Update();
            for (auto& obj : objects) {
                if (glm::distance(m_Nick->GetPosition(), obj->GetPosition()) <= (35.0f + 46.0f) / 2) {
                    m_Nick->OnCollision(obj);
                }
            }
        }

        for (const auto& obj : toRemove) {
            GameWorld::RemoveObject(obj);
            m_Root.RemoveChild(obj);
        }

        if (GameWorld::GetObjects().empty() && m_Nick && m_Nick->GetState() != Nick::State::DIE) {
            if (m_CurrentLevel < 30) {
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

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    LOG_DEBUG("Calling m_Root.Update()");
    m_Root.Update();
}

void App::SetState(State state) {
    if (m_CurrentState == state) return;
    m_CurrentState = state;

    if (state == State::GAMEOVER) {
        LOG_DEBUG("Entering GAME_OVER state");
        m_Root.RemoveChild(m_Nick);
        for (auto& obj : GameWorld::GetObjects()) {
            m_Root.RemoveChild(obj);
        }
        GameWorld::GetObjects().clear();

        // 設置初始圖片並確保位置與可見性
        std::string initialImage = RESOURCE_DIR "/Image/Background/Game_Over_1.png";
        LOG_INFO("Setting fresh overlay to: {}", initialImage);
        m_Overlay->SetImage(initialImage);
        m_Overlay->m_Transform.scale = {720.0f, 720.0f};
        m_Overlay->SetZIndex(10000);
        m_Overlay->SetVisible(true);
    }
}

void App::UpdateFadeAnimation(float deltaTime) {
    if (m_CurrentLevel != -1 || m_CurrentState != State::UPDATE) return;

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
    m_Overlay->SetVisible(levelId == -1);

    if (levelId == 1) {
        if (!m_Nick) {
            m_Nick = std::make_shared<Nick>();
            m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(m_Nick));
            m_Nick->SetState(Nick::State::SPAWN);
        }
        for (auto obj : GameWorld::GetObjects()) {
            m_Root.RemoveChild(obj);
        }
        GameWorld::GetObjects().clear();
        SpawnEnemiesForLevel(levelId);
        LOG_DEBUG("Initialized Level {} with {} enemies", levelId, GameWorld::GetObjects().size());
    } else if (levelId >= 2 && levelId <= 30) {
        for (auto obj : GameWorld::GetObjects()) {
            m_Root.RemoveChild(obj);
        }
        GameWorld::GetObjects().clear();
        SpawnEnemiesForLevel(levelId);
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
                    enemy = std::make_shared<RedDemon>(pos);
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