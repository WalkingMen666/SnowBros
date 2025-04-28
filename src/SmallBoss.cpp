#include "SmallBoss.hpp"
#include "Nick.hpp"
#include "Snowball.hpp"
#include "Map.hpp"
#include "App.hpp"
#include <cstdlib>
#include <ctime>

SmallBoss::SmallBoss(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Boss/boss1_small_fall_left.png", pos) {
    LoadAnimations();
    SetState(State::FALL); // 初始為 FALL 狀態
    m_Drawable = m_Animations["fall_left"]; // 初始動畫為 fall_left
    m_TargetDirection = Direction::Left; // 初始面向左邊
    m_Direction = Direction::Left;
    m_InitialVelocityX = -600.0f; // 初始速度
    m_IsEjecting = true; // 啟用彈射階段
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void SmallBoss::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    glm::vec2 newPosition = position;

    if (m_State == EnemyState::Normal) {
        // 初始化移動距離
        float moveDistance = 0.0f;

        // 計算移動距離和狀態
        if (m_IsEjecting) {
            // 彈射階段：向左移動，動畫為 fall_left，保持下墜
            moveDistance = m_InitialVelocityX * deltaTime;
            m_Drawable = m_Animations["fall_left"];
            SetState(State::FALL);
        } else {
            // 正常狀態
            if (!m_IsOnPlatform || m_JumpVelocity != 0.0f) {
                // 下墜中：無水平移動，動畫為 fall_left
                moveDistance = 0.0f;
                m_Drawable = m_Animations["fall_left"];
                SetState(State::FALL);
            } else {
                // 落地後：進入 WALK 狀態並移動
                SetState(State::WALK);
                moveDistance = (m_Direction == Direction::Right) ? m_speed * deltaTime : -m_speed * deltaTime;
                m_Drawable = m_Animations[(m_Direction == Direction::Right) ? "walk_right" : "walk_left"];
            }
        }

        // 執行碰撞檢測和位置更新
        m_IsOnPlatform = false; // 重置，依賴 map_collision_judgement 更新
        newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, position, m_JumpVelocity, m_Gravity, moveDistance, m_IsOnPlatform);

        // 檢查是否退出彈射狀態
        if (m_IsEjecting && m_IsOnPlatform) {
            m_IsEjecting = false; // 結束彈射階段
            m_JumpVelocity = 0.0f; // 重置跳躍速度
            m_Direction = Direction::Left;
            m_TargetDirection = Direction::Left;
            SetState(State::WALK);
            // m_Drawable = m_Animations["walk_left"];
            LOG_DEBUG("Ejection ended: Position: {}, IsOnPlatform: {}", glm::to_string(newPosition), m_IsOnPlatform);
        }

        // 處理方向變化
        if (m_IsChangingDirection) {
            m_Direction = m_TargetDirection;
            m_IsChangingDirection = false;
            if (m_IsOnPlatform && !m_IsEjecting) {
                SetState(State::WALK);
                m_Drawable = m_Animations[(m_Direction == Direction::Right) ? "walk_right" : "walk_left"];
            }
        } else if (!m_IsEjecting) {
            // 正常狀態下的邊界檢查
            if (newPosition.x <= (-Map::MAP_WIDTH * Map::TILE_SIZE + m_Width) / 2) { // 左邊界
                m_IsChangingDirection = true;
                m_TargetDirection = Direction::Right;
                // m_Direction = Direction::Right;
                m_Drawable = m_Animations["walk_right"];
                SetState(State::WALK);
            } else if (newPosition.x >= (Map::MAP_WIDTH * Map::TILE_SIZE - m_Width) / 2) { // 右邊界
                App::GetInstance().AddRemovingObject(shared_from_this());
            } else if (!m_IsOnPlatform) { // 下墜不移動
                m_IsChangingDirection = true;
                m_TargetDirection = Direction::Left;
                SetState(State::FALL);
                // moveDistance = 0.0f;
            }
        }
        SetPosition(newPosition);
        if (m_CurrentState == State::WALK) {
             m_Drawable = m_Animations[(m_Direction == Direction::Right) ? "walk_right" : "walk_left"];
        }

    } else if (m_State == EnemyState::Snowball) {
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
        m_DeathTimer += deltaTime;
        if (!m_HasLanded) {
            newPosition = GameWorld::map_collision_judgement(m_Width, m_Height, newPosition, m_DeathVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            if (m_DeathTimer >= m_DeathDuration && (m_IsOnPlatform || newPosition.y <= -310.0f + m_Height / 2)) {
                newPosition.y = -310.0f + m_Height / 2;
                m_DeathVelocity = 0.0f;
                m_HasLanded = true;
                m_DeathTimer = 0.0f;
            }
        } else {
            newPosition = GameWorld::map_collision_judgement(20, 20, newPosition, m_DeathVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
            if (m_DeathTimer >= m_LandingDuration) {
                App::GetInstance().AddRemovingObject(shared_from_this());
            }
        }
        SetPosition(newPosition);
    }

    // 碰撞檢測
    if (auto nick = App::GetInstance().GetNick()) {
        if (glm::distance(GetPosition(), nick->GetPosition()) < (nick->GetCharacterWidth() + GetCharacterWidth()) / 2) {
            OnCollision(nick);
        }
    }
}

void SmallBoss::OnHit() {
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

void SmallBoss::Die() {
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
    }
}

std::pair<float, float> SmallBoss::GetSizeForMeltStage() const {
    return m_State == EnemyState::Snowball ? std::make_pair(20.0f, 20.0f) : std::make_pair(m_Width, m_Height);
}

void SmallBoss::SetState(State state) {
    m_CurrentState = state;

    switch (state) {
        case State::STAND:
            SetAnimation((m_Direction == Direction::Right) ? "stand_right" : "stand_left");
            break;
        case State::WALK:
            SetAnimation((m_Direction == Direction::Right) ? "walk_right" : "walk_left");
            break;
        case State::FALL:
            SetAnimation("fall_left");
            break;
        case State::DIE:
            break;
    }
}

void SmallBoss::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_State == EnemyState::Normal) {
            nick->Die();
        }
    }
}

void SmallBoss::LoadAnimations() {
    m_Animations["stand_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "boss1_small_stand_left.png"}, false, 500, true, 0);
    m_Animations["stand_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "boss1_small_stand_right.png"}, false, 500, true, 0);
    m_Animations["walk_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "boss1_small_stand_left.png", BASE_PATH + "boss1_small_walk_left_2.png"}, false, 200, true, 0);
    m_Animations["walk_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "boss1_small_stand_right.png", BASE_PATH + "boss1_small_walk_right_2.png"}, false, 200, true, 0);
    m_Animations["fall_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "boss1_small_fall_left.png"}, false, 500, false, 0);
    m_Animations["fall_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "boss1_small_fall_right.png"}, false, 500, false, 0);
}