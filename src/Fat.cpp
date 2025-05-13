#include "Fat.hpp"
#include "Nick.hpp"
#include "Snowball.hpp"
#include "Map.hpp"
#include "App.hpp"
#include "FatBullet.hpp"
#include <cstdlib>
#include <ctime>

Fat::Fat(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Enemies/fat_stand.png", pos) {
    LoadAnimations();
    SetState(State::STAND);
    m_Drawable = m_Animations["stand"];
    m_TargetDirection = m_Direction;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Fat::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    glm::vec2 newPosition = position;

    if (m_State == EnemyState::Normal) {
        m_IsOnPlatform = false;
        m_ActionTimer += deltaTime;
        m_Direction = m_TargetDirection;

        if (m_ActionTimer >= ACTION_DELAY && m_CurrentState == State::STAND) {
            SetState(State::WALK);
        }

        float moveSpeed = (m_CurrentState == State::STAND) ? 0.0f : m_speed * deltaTime;
        float moveDistance = (m_Direction == Direction::Right) ? moveSpeed : -moveSpeed;

        newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, position, m_JumpVelocity, m_Gravity, moveDistance, m_IsOnPlatform);

        if (m_ActionTimer >= ACTION_DELAY) {
            if (m_IsChangingDirection) {
                m_Direction = m_TargetDirection;
                m_IsChangingDirection = false;
                m_hasAttacked = false;
                SetState(State::WALK);
            } else {
                int action = std::rand() % 10;
                if (action < 6) {
                    SetState(State::WALK);
                } else if (!m_hasAttacked) {
                    SetState(State::ATTACK);
                    m_ActionTimer = 0.0f;
                    m_hasAttacked = true;
                    Attack();
                }
                m_IsActing = false;
            }
            m_ActionTimer = 0.0f;
        }

        if (GameWorld::CollisionToWall(newPosition, m_Width, m_Height, m_IsOnPlatform)) {
            m_IsChangingDirection = true;
            m_TargetDirection = (m_Direction == Direction::Right) ? Direction::Left : Direction::Right;
            SetState(State::ON_EDGE);
            m_ActionTimer = 0.0f;
        }
        
        if (m_CurrentState == State::WALK || m_CurrentState == State::STAND) {
            SetPosition(newPosition);
        }
    } else if (m_State == EnemyState::Dead) {
        m_DeathTimer += deltaTime;
        if (!m_HasLanded) {
            newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, newPosition, m_DeathVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            if (m_DeathTimer >= m_DeathDuration && (m_IsOnPlatform || newPosition.y <= m_GroundLevel)) {
                if (newPosition.y < m_GroundLevel) newPosition.y = m_GroundLevel;
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
    if (auto nick = App::GetInstance().GetNick()) {
        if (glm::distance(GetPosition(), nick->GetPosition()) < (nick->GetCharacterWidth() + GetCharacterWidth()) / 2) {
            OnCollision(nick);
        }
    }
}

void Fat::Attack() {
    LOG_INFO("Fat attacks");

    // Define the angles for the bullets (210°, 240°, 300°, 330°)
    std::vector<float> angles = {210.0f, 240.0f, 300.0f, 330.0f};

    // Get Fat's current position
    glm::vec2 pos = GetPosition();

    // Create and add four bullets to the game world
    for (float angle : angles) {
        auto bullet = std::make_shared<FatBullet>(pos, angle);
        App::GetInstance().AddPendingObject(bullet);
    }
}

void Fat::OnHit() {
    LOG_INFO("Fat can't be hit");
}

void Fat::Die() {
    if (m_State == EnemyState::Snowball) {
        m_Snowball->SetVisible(false);
        App::GetInstance().AddRemovingObject(m_Snowball);
        App::GetInstance().AddRemovingObject(shared_from_this());
    } else {
        SetState(State::DIE);
        m_State = EnemyState::Dead;
        m_DeathTimer = 0.0f;
        m_HasLanded = false;
        m_DeathVelocity = 450.0f;
        SetAnimation("die_flying"); // 開始飛行動畫
    }
}

void Fat::SetState(State state) {
    if (m_CurrentState == state) return;
    m_CurrentState = state;

    switch (state) {
        case State::STAND:
            SetAnimation("stand");
            break;
        case State::WALK:
            SetAnimation((m_Direction == Direction::Right) ? "walk_right" : "walk_left");
            break;
        case State::ATTACK:
            SetAnimation("attack");
            break;
        case State::ON_EDGE:
            SetAnimation((m_Direction == Direction::Right) ? "on_edge_right" : "on_edge_left");
            break;
        case State::DIE:
            // 這裡不直接設置動畫，因為 Die() 會處理
            break;
    }
}

void Fat::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_State == EnemyState::Normal) {
            nick->Die();
        }
    }
}

void Fat::LoadAnimations() {
    m_Animations["stand"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "fat_stand.png"}, false, 500, true, 0);
    m_Animations["walk_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "fat_walk_left.png"}, false, 200, true, 0);
    m_Animations["walk_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "fat_walk_right.png"}, false, 200, true, 0);
    m_Animations["attack"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "fat_att_1.png", BASE_PATH + "fat_att_2.png"}, false, 200, false, 0);
    m_Animations["on_edge_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "fat_walk_right.png", BASE_PATH + "fat_walk_left.png", BASE_PATH + "fat_walk_right.png", BASE_PATH + "fat_walk_left.png", BASE_PATH + "fat_walk_right.png"}, false, 200, false, 0);
    m_Animations["on_edge_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "fat_walk_left.png", BASE_PATH + "fat_walk_right.png", BASE_PATH + "fat_walk_left.png", BASE_PATH + "fat_walk_right.png", BASE_PATH + "fat_walk_left.png"}, false, 200, false, 0);
    m_Drawable = m_Animations["stand"];
}