#include "Frog.hpp"
#include "FrogBullet.hpp"
#include "Nick.hpp"
#include "Snowball.hpp"
#include "Map.hpp"
#include "App.hpp"
#include <cstdlib>
#include <ctime>

Frog::Frog(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Enemies/green_stand_right.png", pos) {
    LoadAnimations();
    SetState(State::STAND);
    m_Drawable = m_Animations["stand_right"];
    m_TargetDirection = m_Direction;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Frog::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    glm::vec2 newPosition = position;
    m_Direction = m_TargetDirection;

    if (m_State == EnemyState::Normal) {
        m_IsOnPlatform = false;
        m_ActionTimer += deltaTime;
        if (m_CurrentState == State::ATTACK) {
            m_AttackTimer += deltaTime;
            if (m_AttackTimer >= m_AttackDuration) {
                SetState(State::STAND);
                m_AttackTimer = 0.0f;
            }
        } else {
            float moveSpeed = (m_CurrentState == State::STAND || m_CurrentState == State::ATTACK) ? 0.0f : m_speed * deltaTime;
            float moveDistance = (m_Direction == Direction::Right) ? moveSpeed : -moveSpeed;

            newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, position, m_JumpVelocity, m_Gravity, moveDistance, m_IsOnPlatform);

            // 以下為原有的移動、跳躍、掉落等邏輯，保持不變
            if (m_IsActing || m_IsChangingDirection) {
                if (m_ActionTimer >= ACTION_DELAY) {
                    if (m_IsChangingDirection) {
                        m_Direction = m_TargetDirection;
                        m_IsChangingDirection = false;
                    }
                    if (m_IsActing) {
                        int action = std::rand() % 10; // 隨機選擇動作
                        if (action < 2) {
                            m_JumpVelocity = m_JumpInitialVelocity;
                            SetState(State::JUMP);
                        } else if (action < 7) {
                            SetState(State::WALK);
                        } else {
                            SetState(State::ATTACK);
                            int dir = std::rand() % 4;
                            FrogBullet::Direction bulletDir;
                            std::string animKey;
                            switch (dir) {
                                case 0:
                                    bulletDir = FrogBullet::Direction::Up;
                                    animKey = "attack_up";
                                    break;
                                case 1:
                                    bulletDir = FrogBullet::Direction::Down;
                                    animKey = "attack_down";
                                    break;
                                case 2:
                                    bulletDir = FrogBullet::Direction::Left;
                                    animKey = "attack_left";
                                    break;
                                case 3:
                                    bulletDir = FrogBullet::Direction::Right;
                                    animKey = "attack_right";
                                    break;
                            }
                            SetAnimation(animKey);
                            auto bullet = std::make_shared<FrogBullet>(position, bulletDir);
                            App::GetInstance().AddPendingObject(bullet); // 正確加入到 GameWorld 和 m_Root
                            m_AttackTimer = 0.0f;
                        }
                        m_IsActing = false;
                    }
                }
            } else {
                if ((m_CurrentState == State::WALK || m_CurrentState == State::STAND) && m_ActionTimer >= 1.0f) {
                    int decision = std::rand() % 10;
                    if (decision < 1) {
                        m_IsChangingDirection = true;
                        m_TargetDirection = (m_Direction == Direction::Right) ? Direction::Left : Direction::Right;
                        SetState(State::STAND);
                    } else {
                        m_IsActing = true;
                    }
                    m_ActionTimer = 0.0f;
                }

                if (newPosition.x >= (Map::MAP_WIDTH * Map::TILE_SIZE - m_Width) / 2) {
                    m_IsChangingDirection = true;
                    m_TargetDirection = Direction::Left;
                    SetState(State::STAND);
                } else if (newPosition.x <= (-Map::MAP_WIDTH * Map::TILE_SIZE + m_Width) / 2) {
                    m_IsChangingDirection = true;
                    m_TargetDirection = Direction::Right;
                    SetState(State::STAND);
                } else if (GameWorld::CollisionToWall(newPosition, m_Width, m_Height - 5, m_IsOnPlatform)) {
                    m_IsChangingDirection = true;
                    m_TargetDirection = (m_Direction == Direction::Right) ? Direction::Left : Direction::Right;
                    SetState(State::STAND);
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
        }
    } else if (m_State == EnemyState::Snowball) {
        // Snowball 狀態邏輯保持不變
        if (m_Snowball) {
            m_Snowball->Update();
            if (m_Snowball->GetSnowballState() == Snowball::SnowballState::Killed) {
                App::GetInstance().AddRemovingObject(m_Snowball);
                m_Snowball = nullptr;
                App::GetInstance().AddRemovingObject(shared_from_this());
            } else {
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
        // Dead 狀態邏輯保持不變
        m_DeathTimer += deltaTime;
        if (!m_HasLanded) {
            newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, newPosition, m_DeathVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            if (m_DeathTimer >= m_DeathDuration && (m_IsOnPlatform || newPosition.y <= m_GroundLevel)) {
                if(newPosition.y < m_GroundLevel) newPosition.y = m_GroundLevel;
                m_DeathVelocity = 0.0f;
                m_HasLanded = true;
                m_DeathTimer = 0.0f;
                SetAnimation("die_landing");
            }
        } else {
            newPosition = GameWorld::map_collision_judgement(46, 35, newPosition, m_DeathVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            if (m_DeathTimer >= m_LandingDuration) {
                App::GetInstance().AddRemovingObject(shared_from_this());
            }
        }
        SetPosition(newPosition);
    }
    if (auto nick = App::GetInstance().GetNick()) {
        if (glm::distance(GetPosition(), nick->GetPosition()) < (nick->GetCharacterWidth() + GetCharacterWidth()) / 2) {
            OnCollision(nick);
        }
    }
}

void Frog::OnHit() {
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

void Frog::Die() {
    if(m_State == EnemyState::Snowball) {
        m_Snowball->SetVisible(false);
        App::GetInstance().AddRemovingObject(m_Snowball);
        App::GetInstance().AddRemovingObject(shared_from_this());
    }else {
        SetState(State::DIE);
        m_State = EnemyState::Dead;
        m_DeathTimer = 0.0f;
        m_HasLanded = false;
        m_DeathVelocity = 450.0f;
        SetAnimation("die_flying");
    }
}

std::pair<float, float> Frog::GetSizeForMeltStage() const {
    return m_State == EnemyState::Snowball ? std::make_pair(42.0f, 44.0f) : std::make_pair(m_Width, m_Height);
}

void Frog::SetState(State state) {
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
        case State::ATTACK:
            // 動畫在 Update 中設置，因為需要隨機方向
            break;
        case State::DIE:
            break;
    }
}

void Frog::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_State == EnemyState::Normal) {
            nick->Die();
        }
    }
}

void Frog::LoadAnimations() {
    m_Animations["stand_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_stand_left.png"}, false, 500, true, 0);
    m_Animations["stand_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_stand_right.png"}, false, 500, true, 0);
    m_Animations["walk_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_walk_left_1.png", BASE_PATH + "green_walk_left_2.png"}, false, 200, true, 0);
    m_Animations["walk_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_walk_right_1.png", BASE_PATH + "green_walk_right_2.png"}, false, 200, true, 0);
    m_Animations["jump_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_jump_left.png"}, false, 300, false, 0);
    m_Animations["jump_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_jump_right.png"}, false, 300, false, 0);
    m_Animations["down_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_down_left.png"}, false, 300, false, 0);
    m_Animations["down_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_down_right.png"}, false, 300, false, 0);
    m_Animations["die_flying"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_die_1.png", BASE_PATH + "green_die_2.png",
                                 BASE_PATH + "green_die_3.png", BASE_PATH + "green_die_4.png"}, false, 250, true, 0);
    m_Animations["die_landing"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_die_5.png"}, false, 400, false, 0);
    m_Animations["attack_up"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_att_up.png"}, false, 500, false, 0);
    m_Animations["attack_down"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_att_down.png"}, false, 500, false, 0);
    m_Animations["attack_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_att_left.png"}, false, 500, false, 0);
    m_Animations["attack_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "green_att_right.png"}, false, 500, false, 0);
    m_Drawable = m_Animations["stand_right"];
}