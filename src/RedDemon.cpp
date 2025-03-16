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
    if (m_State != EnemyState::Normal) return;
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();

    m_IsOnPlatform = false;
    // 更新動作計時器
    m_ActionTimer += deltaTime;
    m_Direction = m_TargetDirection;

    if(m_ActionTimer >= ACTION_DELAY && m_CurrentState == State::STAND) {
        SetState(State::WALK);
    }

    // 根據當前狀態決定移動速度
    float moveSpeed = (m_CurrentState == State::STAND) ? 0.0f : m_speed * deltaTime;
    float moveDistance = (m_Direction == Direction::Right) ? moveSpeed : -moveSpeed;

    // 應用地圖碰撞檢測
    glm::vec2 newPosition = GameWorld::map_collision_judgement(characterWidth, characterHeight, position, m_JumpVelocity, m_Gravity, moveDistance, m_IsOnPlatform);

    if (m_IsActing || m_IsChangingDirection) {
        // 正在執行跳躍或改變方向
        if (m_ActionTimer >= ACTION_DELAY) {
            if (m_IsActing) {
                int action = std::rand() % 5; // 60%: 跳躍向上, 40%: 不動
                if (action > 1) {
                    m_JumpVelocity = m_JumpInitialVelocity;
                    SetState(State::JUMP);
                } else {
                    SetState(State::WALK); // 若不動，恢復走路
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
        // 正常移動狀態
        if (m_CurrentState == State::WALK && m_ActionTimer >= 1.0f) {
            int decision = std::rand() % 10;
            if (decision < 5) { // 50% 機率改變方向
                m_IsChangingDirection = true;
                m_TargetDirection = (m_Direction == Direction::Right) ? Direction::Left : Direction::Right;
                SetState(State::STAND);
                m_ActionTimer = 0.0f;
            } else if (decision < 8) { // 30% 機率切換平台
                m_IsActing = true;
                SetState(State::STAND);
                m_ActionTimer = 0.0f;
            } else {
                m_ActionTimer = 0.0f;
            }
        }

        // 檢查邊界並反向
        if (newPosition.x >= (Map::MAP_WIDTH * Map::TILE_SIZE - characterWidth) / 2) {
            m_IsChangingDirection = true;
            m_TargetDirection = Direction::Left;
            SetState(State::STAND);
            m_ActionTimer = 0.0f;
        } else if (newPosition.x <= (-Map::MAP_WIDTH * Map::TILE_SIZE + characterWidth) / 2) {
            m_IsChangingDirection = true;
            m_TargetDirection = Direction::Right;
            SetState(State::STAND);
            m_ActionTimer = 0.0f;
        } else if (m_CurrentState == State::WALK) {
            if(m_JumpVelocity < 0.0f) SetState(State::FALL);
        }

        // 處理跳躍和下落狀態
        if (m_CurrentState == State::JUMP || m_CurrentState == State::FALL) {
            if (m_IsOnPlatform && m_JumpVelocity <= 0.0f) {
                SetState(State::STAND); // 落地後切換回站立
            }
        }
    }
    SetPosition(newPosition);
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
        nick->Die();
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
    m_Drawable = m_Animations["stand_right"];
}