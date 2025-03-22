#include "RedDemon.hpp"
#include "Nick.hpp"
#include "Snowball.hpp"
#include "Map.hpp"
#include "App.hpp"
#include "Util/Logger.hpp"
#include <cstdlib>
#include <ctime>

RedDemon::RedDemon(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Enemies/red_stand_right.png", pos) {
    LoadAnimations();
    SetState(State::STAND);
    m_Drawable = m_Animations["stand_right"];
    m_TargetDirection = m_Direction;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void RedDemon::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    glm::vec2 newPosition = position;

    LOG_DEBUG("RedDemon Update - State: {}, Position: ({}, {}), JumpVelocity: {}, IsOnPlatform: {}",
              static_cast<int>(m_State), position.x, position.y, m_JumpVelocity, m_IsOnPlatform);

    if (m_State == EnemyState::Normal) {
        m_IsOnPlatform = false;
        m_ActionTimer += deltaTime;
        m_Direction = m_TargetDirection;

        if (m_ActionTimer >= ACTION_DELAY && m_CurrentState == State::STAND) {
            SetState(State::WALK);
        }

        float moveSpeed = (m_CurrentState == State::STAND) ? 0.0f : m_speed * deltaTime;
        float moveDistance = (m_Direction == Direction::Right) ? moveSpeed : -moveSpeed;

        newPosition = GameWorld::map_collision_judgement(46.0f, 46.0f, position, m_JumpVelocity, m_Gravity, moveDistance, m_IsOnPlatform);

        if (m_IsActing || m_IsChangingDirection) {
            if (m_ActionTimer >= ACTION_DELAY) {
                if (m_IsActing) {
                    int action = std::rand() % 5;
                    if (action > 1) {
                        m_JumpVelocity = m_JumpInitialVelocity;
                        SetState(State::JUMP);
                    } else {
                        SetState(State::WALK);
                    }
                    m_IsActing = false;
                } else if (m_IsChangingDirection) {
                    m_Direction = m_TargetDirection;
                    m_IsChangingDirection = false;
                    SetState(State::WALK);
                }
                m_ActionTimer = 0.0f;
            }
        } else {
            if (m_CurrentState == State::WALK && m_ActionTimer >= 1.0f) {
                int decision = std::rand() % 10;
                if (decision < 5) {
                    m_IsChangingDirection = true;
                    m_TargetDirection = (m_Direction == Direction::Right) ? Direction::Left : Direction::Right;
                    SetState(State::STAND);
                    m_ActionTimer = 0.0f;
                } else if (decision < 8) {
                    m_IsActing = true;
                    SetState(State::STAND);
                    m_ActionTimer = 0.0f;
                } else {
                    m_ActionTimer = 0.0f;
                }
            }

            if (newPosition.x >= (Map::MAP_WIDTH * Map::TILE_SIZE - 46.0f) / 2) {
                m_IsChangingDirection = true;
                m_TargetDirection = Direction::Left;
                SetState(State::STAND);
                m_ActionTimer = 0.0f;
            } else if (newPosition.x <= (-Map::MAP_WIDTH * Map::TILE_SIZE + 46.0f) / 2) {
                m_IsChangingDirection = true;
                m_TargetDirection = Direction::Right;
                SetState(State::STAND);
                m_ActionTimer = 0.0f;
            } else if (m_CurrentState == State::WALK) {
                if (m_JumpVelocity < 0.0f) SetState(State::FALL);
            }

            if (m_CurrentState == State::JUMP || m_CurrentState == State::FALL) {
                if (m_IsOnPlatform && m_JumpVelocity <= 0.0f) {
                    SetState(State::STAND);
                }
            }
        }
        SetPosition(newPosition);
    } else if (m_State == EnemyState::Snowball) {
        auto [width, height] = GetSizeForMeltStage();
        float snowballGravity = -800.0f;
        float moveDistance = 0.0f;

        for (auto& obj : GameWorld::GetObjects()) {
            if (auto otherEnemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                if (otherEnemy.get() != this && otherEnemy->GetState() == EnemyState::Snowball) {
                    float distance = glm::distance(position, otherEnemy->GetPosition());
                    if (distance < (width + otherEnemy->GetCharacterWidth()) / 2) {
                        moveDistance = 0.0f;
                        LOG_DEBUG("Snowball collision detected - Position: ({}, {})", position.x, position.y);
                        break;
                    }
                }
            }
        }

        // 修改：確保 m_JumpVelocity 在空中持續累積
        if (!m_IsOnPlatform) {
            m_JumpVelocity += snowballGravity * deltaTime;
            LOG_INFO("Snowball falling - JumpVelocity: {}", m_JumpVelocity);
        } else {
            m_JumpVelocity = 0.0f;
        }
        LOG_INFO("Snowball Pos, m_JumpVelocity, m_IsOnPlatform: {}, {}, {}", position, m_JumpVelocity, m_IsOnPlatform);
        newPosition = GameWorld::map_collision_judgement(width, height, position, m_JumpVelocity, snowballGravity, moveDistance, m_IsOnPlatform);
        LOG_INFO("TEST");
        bool isBeingPushed = false;
        if (auto nick = App::GetInstance().GetNick()) {
            if (glm::distance(position, nick->GetPosition()) < (nick->GetCharacterWidth() + width) / 2 &&
                (Util::Input::IsKeyPressed(Util::Keycode::A) || Util::Input::IsKeyPressed(Util::Keycode::D))) {
                isBeingPushed = true;
            }
        }
        if (!isBeingPushed) {
            m_MeltTimer += deltaTime;
        }

        if (m_MeltTimer >= m_MeltDuration / 2 && m_MeltStage < 3) {
            SetMeltStage(m_MeltStage + 1);
        }
        if (m_MeltStage == 3 && m_MeltTimer >= m_MeltDuration) {
            m_State = EnemyState::Normal;
            m_HitCount = 0;
            m_MeltStage = 0;
            SetState(State::STAND);
            auto [newWidth, newHeight] = GetSizeForMeltStage();
            float heightDiff = (newHeight - height) / 2;
            newPosition.y += heightDiff;
            newPosition = GameWorld::map_collision_judgement(newWidth, newHeight, newPosition, m_JumpVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            SetPosition(newPosition);
            LOG_DEBUG("RedDemon restored to Normal state, adjusted Position: ({}, {})", newPosition.x, newPosition.y);
        } else {
            SetPosition(newPosition);
        }
    } else if (m_State == EnemyState::Dead) {
        if (std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->GetState() == Util::Animation::State::ENDED) {
            GameWorld::RemoveObject(std::shared_ptr<RedDemon>(this));
        }
    }

    if (auto nick = App::GetInstance().GetNick()) {
        if (glm::distance(GetPosition(), nick->GetPosition()) < (nick->GetCharacterWidth() + GetCharacterWidth()) / 2) {
            OnCollision(nick);
        }
    }
}

void RedDemon::OnHit(Bullet* bullet) {
    if (m_State == EnemyState::Normal) {
        m_HitCount++;
        LOG_DEBUG("RedDemon hit, m_HitCount: {}", m_HitCount);
        if (m_HitCount >= m_MaxHits) {
            m_State = EnemyState::Snowball;
            glm::vec2 position = GetPosition();
            auto [width, height] = GetSizeForMeltStage();
            bool isOnPlatform = m_IsOnPlatform;
            // 修改：在 Snowball 狀態下固定參數
            m_JumpVelocity = 0.0f; // 固定為 0
            float snowballGravity = -800.0f; // 固定為 -800
            glm::vec2 newPosition = GameWorld::map_collision_judgement(width, height, position, m_JumpVelocity, snowballGravity, 0.0f, isOnPlatform);
            LOG_DEBUG("Turning to Snowball - Position: ({}, {}), IsOnPlatform: {}", position.x, position.y, isOnPlatform);
            if (isOnPlatform) {
                SetPosition(newPosition);
            }
            SetMeltStage(3);
        }
    } else if (m_State == EnemyState::Snowball && m_MeltStage > 1) {
        SetMeltStage(m_MeltStage - 1);
    }
}

// 修改：統一雪球大小為 42x44
std::pair<float, float> RedDemon::GetSizeForMeltStage() const {
    if (m_State != EnemyState::Snowball) {
        return {46.0f, 46.0f};
    }
    return {42.0f, 44.0f}; // 修改：所有雪球狀態統一為 42x44
}

void RedDemon::SetState(State state) {
    if (m_CurrentState == state) return;
    m_CurrentState = state;

    switch (state) {
        case State::STAND:
            SetAnimation((m_Direction == Direction::Right) ? "stand_right" : "stand_left");
            break;
        case State::WALK:
            SetAnimation((m_Direction == Direction::Right) ? "walk_right" : "walk_left");
            break;
        case State::JUMP:
            SetAnimation((m_Direction == Direction::Right) ? "jump_right" : "jump_left");
            break;
        case State::FALL:
            SetAnimation((m_Direction == Direction::Right) ? "down_right" : "down_left");
            break;
        case State::DIE:
            SetAnimation("die");
            break;
    }
}

void RedDemon::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_State == EnemyState::Normal) {
            nick->Die();
        }
    } else if (auto snowball = std::dynamic_pointer_cast<Snowball>(other)) {
        if (snowball->IsMoving()) {
            m_State = EnemyState::Dead;
            SetState(State::DIE);
        }
    }
}

void RedDemon::LoadAnimations() {
    m_Animations["stand_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_stand_left.png"}, false, 500, true, 0);
    m_Animations["stand_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_stand_right.png"}, false, 500, true, 0);
    m_Animations["walk_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_walk_left_1.png", BASE_PATH + "red_walk_left_2.png"}, false, 200, true, 0);
    m_Animations["walk_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_walk_right_1.png", BASE_PATH + "red_walk_right_2.png"}, false, 200, true, 0);
    m_Animations["jump_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_jump_left.png"}, false, 300, false, 0);
    m_Animations["jump_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_jump_right.png"}, false, 300, false, 0);
    m_Animations["down_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_down_left.png"}, false, 300, false, 0);
    m_Animations["down_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_down_right.png"}, false, 300, false, 0);
    m_Animations["die"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_die_1.png", BASE_PATH + "red_die_2.png", BASE_PATH + "red_die_3.png",
                                 BASE_PATH + "red_die_4.png", BASE_PATH + "red_die_5.png"}, false, 500, false, 0);
    m_Animations["snowball_roll"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_roll_1.png", BASE_PATH + "snowball_roll_2.png",
                                 BASE_PATH + "snowball_roll_3.png", BASE_PATH + "snowball_roll_4.png"}, false, 200, true, 0);
    m_Animations["snowball_melt_1"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_melt_1.png"}, false, 500, true, 0);
    m_Animations["snowball_melt_2"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_melt_2.png"}, false, 500, true, 0);
    m_Animations["snowball_melt_3"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_melt_3.png"}, false, 500, true, 0);
    m_Drawable = m_Animations["stand_right"];
}