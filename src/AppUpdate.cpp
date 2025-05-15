#include "App.hpp"
#include "Nick.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameWorld.hpp"
#include "RedDemon.hpp"
#include "Frog.hpp"
#include "Monkey.hpp"
#include "FrogBullet.hpp"
#include "FatBullet.hpp"
#include "Enemy.hpp"
#include "Fat.hpp"
#include "Bullet.hpp"
#include "Boss.hpp"
#include "Boss2.hpp"
#include "SmallBoss.hpp"
#include "SmallBoss2.hpp"

void App::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    UpdateFadeAnimation(deltaTime);

    if (m_PRM && m_PRM->GetPhase() != m_CurrentLevel) {
        m_CurrentLevel = m_PRM->GetPhase();
    }

    if (m_CurrentState == State::GAMEOVER) {
        m_GameOverTimer += deltaTime;

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
            m_Overlay->SetImage(path);
            m_Overlay->m_Transform.translation = {0.0f, 0.0f};
            m_Overlay->m_Transform.scale = {1.0f, 1.0f};
            m_Overlay->SetVisible(true);
            m_Overlay->SetZIndex(10);
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
            newPosition = {0.0f, -325.0f};
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

        if(Util::Input::IsKeyDown(Util::Keycode::H)) {
            LOG_INFO("Add one health");
            m_Nick->AddHealth();
        }
        if(Util::Input::IsKeyDown(Util::Keycode::I)) {
            LOG_INFO("Nick is invincible now");
            m_Nick->CheatInvincible();
        }
    }

    if (m_CurrentLevel == -1) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_PRM->NextPhase();
            InitializeLevel(0);
        }
    } else if (m_CurrentLevel == 0) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_PRM->NextPhase();
            InitializeLevel(1);
        }
    } else if (m_CurrentLevel >= 1 && m_CurrentLevel <= 30) {
        auto& objects = GameWorld::GetObjects();

        for (auto& obj : objects) {
            obj->Update();
            if (auto bullet = std::dynamic_pointer_cast<Bullet>(obj)) {
                if (m_Nick) {
                    glm::vec2 bulletPos = bullet->GetPosition();
                    glm::vec2 nickPos = m_Nick->GetPosition();
                    float bulletLeft = bulletPos.x - bullet->GetWidth() / 2;
                    float bulletRight = bulletPos.x + bullet->GetWidth() / 2;
                    float bulletTop = bulletPos.y + bullet->GetHeight() / 2;
                    float bulletBottom = bulletPos.y - bullet->GetHeight() / 2;
                    float nickLeft = nickPos.x - m_Nick->GetCharacterWidth() / 2;
                    float nickRight = nickPos.x + m_Nick->GetCharacterWidth() / 2;
                    float nickTop = nickPos.y + m_Nick->GetCharacterHeight() / 2;
                    float nickBottom = nickPos.y - m_Nick->GetCharacterHeight() / 2;

                    if (bulletRight > nickLeft && bulletLeft < nickRight &&
                        bulletTop > nickBottom && bulletBottom < nickTop) {
                        bullet->OnCollision(m_Nick);
                    }
                }

                for (auto& other : objects) {
                    if (bullet != other) {
                        glm::vec2 bulletPos = bullet->GetPosition();
                        glm::vec2 otherPos = other->GetPosition();
                        float bulletLeft = bulletPos.x - bullet->GetWidth() / 2;
                        float bulletRight = bulletPos.x + bullet->GetWidth() / 2;
                        float bulletTop = bulletPos.y + bullet->GetHeight() / 2;
                        float bulletBottom = bulletPos.y - bullet->GetHeight() / 2;
                        float otherLeft = otherPos.x - other->GetWidth() / 2;
                        float otherRight = otherPos.x + other->GetWidth() / 2;
                        float otherTop = otherPos.y + other->GetHeight() / 2;
                        float otherBottom = otherPos.y - other->GetHeight() / 2;

                        if (bulletRight > otherLeft && bulletLeft < otherRight &&
                            bulletTop > otherBottom && bulletBottom < otherTop) {
                            bullet->OnCollision(other);
                            if (auto snowball = std::dynamic_pointer_cast<Snowball>(other)) {
                                snowball->OnHit();
                            }
                        }
                    }
                }

                if (bullet->IsMarkedForRemoval()) {
                    AddRemovingObject(obj);
                }
            }
        }

        if (m_Nick) {
            m_Nick->Update();
            for (auto& obj : objects) {
                if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                    if (glm::distance(m_Nick->GetPosition(), enemy->GetPosition()) <= (m_Nick->GetCharacterWidth() + enemy->GetCharacterWidth()) / 2) {
                        m_Nick->OnCollision(obj);
                    }
                }
            }
        }

        // 添加待處理的物件
        for (const auto& obj : m_PendingObjects) {
            GameWorld::AddObject(obj);
            m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(obj));
        }m_PendingObjects.clear();

        for (const auto& obj : m_RemovingObjects) {
            GameWorld::RemoveObject(obj);
            m_Root.RemoveChild(obj);
        }m_RemovingObjects.clear();

        if (GameWorld::GetObjects().empty() && m_Nick && m_Nick->GetState() != Nick::State::DIE) {
            if (m_CurrentLevel < 30) {
                for (auto obj : GameWorld::GetObjects()) {
                    m_Root.RemoveChild(obj);
                }
                GameWorld::GetObjects().clear();

                m_LevelingTimer += deltaTime;
                if (m_LevelingTimer < m_LevelingDuration) {
                    m_Overlay->SetImage(RESOURCE_DIR "/Image/Floor/FLOOR" + std::to_string(m_CurrentLevel + 1) + ".png");
                    m_Overlay->SetVisible(true);
                    m_nickLives->SetVisible(false);
                }else {
                    m_LevelingTimer = 0.0f;
                    m_Overlay->SetVisible(false);
                    m_PRM->NextPhase();
                    InitializeLevel(m_PRM->GetPhase());
                    m_Nick->SetPosition({0.0f, -325.0f});
                    m_Nick->SetState(Nick::State::SPAWN);
                }
            } else {
                m_CurrentState = State::END;
            }
        }
    }

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    UpdateUI();
    m_Root.Update();
}

// 以下保持不變
void App::SetState(State state) {
    if (m_CurrentState == state) return;
    m_CurrentState = state;

    if (state == State::GAMEOVER) {
        m_Root.RemoveChild(m_Nick);
        for (auto& obj : GameWorld::GetObjects()) {
            m_Root.RemoveChild(obj);
        }
        GameWorld::GetObjects().clear();

        std::string initialImage = RESOURCE_DIR "/Image/Background/black/black50.png";
        m_Overlay->SetImage(initialImage);
        // m_Overlay->m_Transform.scale = {720.0f, 720.0f};
        m_Overlay->SetZIndex(10);
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
    m_nickLives->SetVisible(levelId > 0);

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
    } else if (levelId >= 2 && levelId <= 30) {
        for (auto obj : GameWorld::GetObjects()) {
            m_Root.RemoveChild(obj);
        }
        GameWorld::GetObjects().clear();
        SpawnEnemiesForLevel(levelId);
    }
}

void App::SpawnEnemiesForLevel(int levelId) {
    for (const auto& config : m_LevelConfigs) {
        if (config.levelId == levelId) {
            for (const auto& [enemyType, pos] : config.enemies) {
                std::shared_ptr<Enemy> enemy;
                if (enemyType == "RedDemon") {
                    enemy = std::make_shared<RedDemon>(pos);
                }else if (enemyType == "Frog") {
                    enemy = std::make_shared<Frog>(pos);
                }else if (enemyType == "Monkey") {
                    enemy = std::make_shared<Monkey>(pos);
                }else if (enemyType == "Fat") {
                    enemy = std::make_shared<Fat>(pos);
                }else if (enemyType == "Boss") {
                    enemy = std::make_shared<Boss>(pos);
                }else if (enemyType == "Boss2") {
                    enemy = std::make_shared<Boss2>(pos);
                }else if (enemyType == "SmallBoss") {
                    enemy = std::make_shared<SmallBoss>(pos);
                }else if (enemyType == "SmallBoss2") {
                    enemy = std::make_shared<SmallBoss2>(pos);
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

void App::UpdateUI() {
    if(m_Nick) {
        int lives = m_Nick->GetLives();
        m_nickLives->SetImage(RESOURCE_DIR "/Image/Text/red_" + std::to_string(lives) + ".png");
    }
}
