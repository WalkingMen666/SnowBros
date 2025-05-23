#include "Monkey.hpp"
#include "Nick.hpp"
#include "Snowball.hpp"
#include "Map.hpp"
#include "App.hpp"
#include <cstdlib>
#include <ctime>

Monkey::Monkey(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Enemies/monkey_stand_right.png", pos) {
    LoadAnimations();
    SetState(State::STAND);
    m_Drawable = m_Animations["stand_right"];
    m_TargetDirection = m_Direction;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Monkey::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    glm::vec2 newPosition = position;

    if (m_State == EnemyState::Normal) {
        m_IsOnPlatform = false;
        m_ActionTimer += deltaTime;
        m_Direction = m_TargetDirection;

        // Handle attack state
        if (m_IsAttacking) {
            m_AttackTimer += deltaTime;
            if (m_AttackTimer >= m_AttackDuration) {
                m_IsAttacking = false;
                m_AttackTimer = 0.0f;

                // Return to jump or fall state after attack
                if (m_JumpVelocity > 0.0f) {
                    SetState(State::JUMP);
                } else if (m_JumpVelocity < 0.0f) {
                    SetState(State::FALL);
                } else {
                    SetState(State::STAND);
                }
            }
            // Don't move during attack animation
        } else {
            if (m_ActionTimer >= ACTION_DELAY && m_CurrentState == State::STAND) {
                SetState(State::WALK);
            }

            float moveSpeed = (m_CurrentState == State::STAND) ? 0.0f : m_speed * deltaTime;
            float moveDistance = (m_Direction == Direction::Right) ? moveSpeed : -moveSpeed;

            newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, position, m_JumpVelocity, m_Gravity, moveDistance, m_IsOnPlatform);

            // Move the platform drop mechanism after map_collision_judgement
            if (m_CurrentState == State::STAND && m_IsOnPlatform && abs(position.y - m_GroundLevel) >= 10 && (std::rand() % 100 < 1)) {
                // 20% chance to fall through platform when standing
                m_JumpVelocity = -10.0f; // Small negative velocity to start falling
                SetState(State::FALL);
                // Skip one collision check to allow falling through
                position.y -= 2.0f;
                newPosition = position;
            }

            // Determine if monkey should attack during jump or fall
            if (!m_hasAttacked && (m_CurrentState == State::JUMP || m_CurrentState == State::FALL) &&
                !m_IsAttacking && (std::rand() % 100 < 40)) {
                m_IsAttacking = true;
                m_hasAttacked = true;
                m_AttackTimer = 0.0f;
                SetState(State::ATTACK);
            }

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

                if (newPosition.x >= (Map::MAP_WIDTH * Map::TILE_SIZE - m_Width) / 2) {
                    m_IsChangingDirection = true;
                    m_TargetDirection = Direction::Left;
                    SetState(State::STAND);
                    m_ActionTimer = 0.0f;
                } else if (newPosition.x <= (-Map::MAP_WIDTH * Map::TILE_SIZE + m_Width) / 2) {
                    m_IsChangingDirection = true;
                    m_TargetDirection = Direction::Right;
                    SetState(State::STAND);
                    m_ActionTimer = 0.0f;
                } else if (GameWorld::CollisionToWall(newPosition, m_Width, m_Height - 5, m_IsOnPlatform)) {
                    m_IsChangingDirection = true;
                    m_TargetDirection = (m_Direction == Direction::Right) ? Direction::Left : Direction::Right;
                    SetState(State::STAND);
                    m_ActionTimer = 0.0f;
                } else if (m_CurrentState == State::WALK) {
                    if (m_JumpVelocity < 0.0f) SetState(State::FALL);
                }

                if (m_CurrentState == State::JUMP || m_CurrentState == State::FALL) {
                    if (m_IsOnPlatform && m_JumpVelocity <= 0.0f) {
                        SetState(State::STAND);
                        m_hasAttacked = false;
                    }
                }
            }
            SetPosition(newPosition);
        }
    } else if (m_State == EnemyState::Snowball) {
        if (m_Snowball) {
            m_Snowball->Update();
            if (m_Snowball->GetSnowballState() == Snowball::SnowballState::Killed) {
                App::GetInstance().AddRemovingObject(m_Snowball);
                m_Snowball = nullptr;
                App::GetInstance().AddRemovingObject(shared_from_this());
            } else {
                if(m_Snowball && m_Snowball->GetSnowballState() == Snowball::SnowballState::Static) {
                    // 检查是否有其他处于kicked状态的雪球与此雪球碰撞
                    Direction kickDirection = Direction::Right; // 默认方向
                    bool collisionDetected = false;
                    
                    // 获取所有游戏对象
                    auto& objects = GameWorld::GetObjects();
                    
                    // 获取当前雪球的位置和大小
                    glm::vec2 currentSnowballPos = m_Snowball->GetPosition();
                    float currentSnowballLeft = currentSnowballPos.x - m_Snowball->GetWidth() / 2;
                    float currentSnowballRight = currentSnowballPos.x + m_Snowball->GetWidth() / 2;
                    float currentSnowballTop = currentSnowballPos.y + m_Snowball->GetHeight() / 2;
                    float currentSnowballBottom = currentSnowballPos.y - m_Snowball->GetHeight() / 2;
                    
                    for (auto& obj : objects) {
                        // 只检查其他敌人的雪球，且必须是kicked状态
                        if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                            if (enemy != shared_from_this() && enemy->GetSnowball() && 
                                enemy->GetSnowball()->GetSnowballState() == Snowball::SnowballState::Kicked) {
                                
                                // 获取滚动雪球的位置和大小
                                auto rollingSnowball = enemy->GetSnowball();
                                glm::vec2 rollingSnowballPos = rollingSnowball->GetPosition();
                                float rollingSnowballLeft = rollingSnowballPos.x - rollingSnowball->GetWidth() / 2;
                                float rollingSnowballRight = rollingSnowballPos.x + rollingSnowball->GetWidth() / 2;
                                float rollingSnowballTop = rollingSnowballPos.y + rollingSnowball->GetHeight() / 2;
                                float rollingSnowballBottom = rollingSnowballPos.y - rollingSnowball->GetHeight() / 2;
                                
                                // 检查两个雪球是否碰撞
                                if (rollingSnowballRight > currentSnowballLeft && 
                                    rollingSnowballLeft < currentSnowballRight &&
                                    rollingSnowballTop > currentSnowballBottom && 
                                    rollingSnowballBottom < currentSnowballTop) {
                                    
                                    // 发生了碰撞，使用滚动雪球的位置确定方向
                                    kickDirection = rollingSnowballPos.x > currentSnowballPos.x ? 
                                                Direction::Left : Direction::Right;
                                    collisionDetected = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (collisionDetected) {
                        m_Snowball->OnKick(kickDirection);
                    }
                }
                SetVisible(false);
                SetPosition(m_Snowball->GetPosition());
                if (m_Snowball->IsMelted()) {
                    m_State = EnemyState::Normal;
                    m_HitCount = 0;
                    m_MeltStage = 0;
                    SetState(State::STAND);
                    SetVisible(true);

                    float heightDiff = (GetCharacterHeight() - m_Snowball->GetHeight()) / 2;
                    newPosition = m_Snowball->GetPosition();
                    newPosition.y += heightDiff;
                    newPosition = GameWorld::map_collision_judgement(GetCharacterWidth(), GetCharacterHeight(), newPosition, m_JumpVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
                    SetPosition(newPosition);

                    App::GetInstance().AddRemovingObject(m_Snowball);
                    m_Snowball = nullptr;
                }
            }
        }
    } else if (m_State == EnemyState::Dead) {
        m_DeathTimer += deltaTime;
        if (!m_HasLanded) {
            newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, newPosition, m_DeathVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            if (m_DeathTimer >= m_DeathDuration && (m_IsOnPlatform || newPosition.y <= m_GroundLevel)) {
                if(newPosition.y < m_GroundLevel) newPosition.y = m_GroundLevel;
                m_DeathVelocity = 0.0f;
                m_HasLanded = true;
                m_DeathTimer = 0.0f; // 重置計時器用於落地等待
                SetAnimation("die_landing"); // 切換到落地動畫
            }
        } else {
            newPosition = GameWorld::map_collision_judgement(46, 35, newPosition, m_DeathVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            if (m_DeathTimer >= m_LandingDuration) {
                App::GetInstance().AddRemovingObject(shared_from_this());
            }
        }
        SetPosition(newPosition);
    }
    if (m_MaxHealth <= 0 && m_State != EnemyState::Dead && m_State != EnemyState::Snowball) {
        Die();
    }
    if (auto nick = App::GetInstance().GetNick()) {
        if (glm::distance(GetPosition(), nick->GetPosition()) < (nick->GetCharacterWidth() + GetCharacterWidth()) / 2) {
            OnCollision(nick);
        }
    }
}

void Monkey::OnHit() {
    if (m_State == EnemyState::Normal) {
        if (++m_HitCount >= m_MaxHits) {
            m_State = EnemyState::Snowball;
            m_Snowball = std::make_shared<Snowball>(GetPosition(), shared_from_this());
            App::GetInstance().AddPendingObject(m_Snowball);
            m_JumpVelocity = 0.0f;
            m_HitCount = 0;
            SetVisible(false);
        }
    } else if (m_State == EnemyState::Snowball) {
        m_Snowball->OnHit();
    }
}

void Monkey::Die() {
    SetState(State::DIE);
    m_State = EnemyState::Dead;
    m_DeathTimer = 0.0f;
    m_HasLanded = false;
    m_DeathVelocity = 450.0f;
    SetAnimation("die_flying"); // 開始飛行動畫
}

std::pair<float, float> Monkey::GetSizeForMeltStage() const {
    return m_State == EnemyState::Snowball ? std::make_pair(42.0f, 44.0f) : std::make_pair(m_Width, m_Height);
}

void Monkey::SetState(State state) {
    if (m_CurrentState == state) return;

    // If we're in attack state, don't change to another movement state until animation completes
    if (m_IsAttacking && (state == State::JUMP || state == State::FALL || state == State::WALK || state == State::STAND)) {
        return;
    }

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
        case State::ATTACK:
            SetAnimation((m_Direction == Direction::Right) ? "att_right" : "att_left");
            m_IsAttacking = true;
            m_AttackTimer = 0.0f;
            break;
        case State::DIE:
            // 這裡不直接設置動畫，因為 Die() 會處理
            break;
    }
}

void Monkey::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_State == EnemyState::Normal) {
            nick->Die();
        }
    }
}

void Monkey::LoadAnimations() {
    m_Animations["stand_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_stand_left.png"}, false, 500, true, 0);
    m_Animations["stand_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_stand_right.png"}, false, 500, true, 0);
    m_Animations["walk_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_walk_left_1.png", BASE_PATH + "monkey_walk_left_2.png"}, false, 200, true, 0);
    m_Animations["walk_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_walk_right_1.png", BASE_PATH + "monkey_walk_right_2.png"}, false, 200, true, 0);
    m_Animations["jump_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_jump_left.png"}, false, 300, false, 0);
    m_Animations["jump_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_jump_right.png"}, false, 300, false, 0);
    m_Animations["down_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_down_left.png"}, false, 300, false, 0);
    m_Animations["down_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_down_right.png"}, false, 300, false, 0);
    m_Animations["die_flying"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_die_1.png", BASE_PATH + "monkey_die_2.png",
                                 BASE_PATH + "monkey_die_3.png", BASE_PATH + "monkey_die_4.png"}, false, 250, true, 0);
    m_Animations["die_landing"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_die_5.png"}, false, 400, false, 0);
    m_Animations["att_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_att_left.png"}, false, 300, false, 0);
    m_Animations["att_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "monkey_att_right.png"}, false, 300, false, 0);
    m_Drawable = m_Animations["stand_right"];
}