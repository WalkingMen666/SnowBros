#include "Boss.hpp"
#include "Nick.hpp"
#include "SmallBoss.hpp"

Boss::Boss(const glm::vec2& pos)
    : Enemy(RESOURCE_DIR "/Image/Character/Boss/boss1_stand.png", pos) {
    m_MaxHits = 999;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    m_RandomJumpLimit = 3 + std::rand() % 8;
    LoadAnimations();
    SwitchAnimation(BossState::Stand);
}

void Boss::LoadAnimations() {
    m_Animations["boss1_stand"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{RESOURCE_DIR "/Image/Character/Boss/boss1_stand.png"}, 100, true);
    m_Animations["boss1_jump"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{RESOURCE_DIR "/Image/Character/Boss/boss1_jump.png"}, 1000, false);
    m_Animations["boss1_lean"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{RESOURCE_DIR "/Image/Character/Boss/boss1_lean.png"}, 100, false);
    m_Animations["boss1_down"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{RESOURCE_DIR "/Image/Character/Boss/boss1_down.png"}, 100, false);
    m_Animations["boss1_die"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{RESOURCE_DIR "/Image/Character/Boss/boss1_die.png"}, 100, false);
}

void Boss::SwitchAnimation(BossState state) {
    std::string key;
    bool looping = false;
    float newHeight = m_BossHeight;
    switch (state) {
        case BossState::Stand:
            key = "boss1_stand";
            looping = true;
            newHeight = 284.0f;
            break;
        case BossState::Jump:
            key = "boss1_jump";
            newHeight = 284.0f;
            break;
        case BossState::Lean:
            key = "boss1_lean";
            newHeight = 240.0f;
            break;
        case BossState::Down:
            key = "boss1_down";
            newHeight = 192.0f;
            break;
        case BossState::Die:
            key = "boss1_die";
            newHeight = 192.0f;
            break;
        case BossState::Dead:
            return;
    }
    SetAnimation(key);
    auto anim = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
    if (anim) {
        anim->SetLooping(looping);
        anim->Play();
        glm::vec2 position = GetPosition();
        // 調整位置，保持底部對齊當前表面
        if (m_IsOnPlatform) {
            position.y = -50.0f + newHeight / 2.0f; // 平台底部 -50
        } else {
            position.y = -310.0f + newHeight / 2.0f; // 地面底部 -310
        }
        SetPosition(position);
        m_CurrentHeight = newHeight;
    } else {
        LOG_ERROR("Failed to switch animation to {}", key);
    }
}

void Boss::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();

    auto prm = App::GetPRM();
    if (!prm || prm->GetPhase() != 10) return;

    if (m_CurrentState == BossState::Dead) return;

    if (m_CurrentState == BossState::Stand || m_CurrentState == BossState::Jump) {
        m_SmallJumpTimer += deltaTime;

        // LOG_DEBUG("Boss position: {}, State: {}, SmallJumpTimer: {}, SmallJumpCount: {}, RandomJumpLimit: {}, IsOnPlatform: {}, JumpVelocity: {}, IsJumpingDown: {}",
                  // glm::to_string(position), static_cast<int>(m_CurrentState), m_SmallJumpTimer, m_SmallJumpCount, m_RandomJumpLimit, m_IsOnPlatform, m_JumpVelocity, m_IsJumpingDown);

        if (m_CurrentState == BossState::Stand && m_SmallJumpTimer >= m_SmallJumpInterval) {
            m_SmallJumpTimer = 0.0f;
            m_SmallJumpCount++;

            if (m_SmallJumpCount >= m_RandomJumpLimit) {
                m_SmallJumpCount = 0;
                m_RandomJumpLimit = 3 + std::rand() % 8;
                if (position.y > -51.0f) { // 在平台上
                    m_JumpVelocity = 0.0f;
                    m_CurrentState = BossState::Jump;
                    SwitchAnimation(BossState::Jump);
                    m_IsOnPlatform = false;
                    m_IsJumpingDown = true;
                    App::GetInstance().AddPendingObject(std::make_shared<SmallBoss>(glm::vec2(200.0f, 130.0f)));
                    App::GetInstance().AddPendingObject(std::make_shared<SmallBoss>(glm::vec2(200.0f, 50.0f)));
                    App::GetInstance().AddPendingObject(std::make_shared<SmallBoss>(glm::vec2(200.0f, -30.0f)));
                    LOG_INFO("Boss jumping down from platform ({} jumps)", m_RandomJumpLimit);
                } else { // 在地面
                    m_JumpVelocity = m_JumpInitialVelocity;
                    m_CurrentState = BossState::Jump;
                    SwitchAnimation(BossState::Jump);
                    m_IsOnPlatform = false;
                    m_IsJumpingDown = false;
                    LOG_INFO("Boss jumping up to platform ({} jumps)", m_RandomJumpLimit);
                }
            } else {
                m_JumpVelocity = m_JumpInitialVelocity * 0.5f;
                m_CurrentState = BossState::Jump;
                SwitchAnimation(BossState::Jump);
                m_IsOnPlatform = false;
                m_IsJumpingDown = false;
                LOG_INFO("Boss performing small jump ({}/{})", m_SmallJumpCount, m_RandomJumpLimit);
            }
        }

        if (m_CurrentState == BossState::Jump) {
            m_JumpVelocity += m_Gravity * deltaTime;
            position.y += m_JumpVelocity * deltaTime;

            float moveDistance = 0.0f;
            glm::vec2 newPosition = GameWorld::map_collision_judgement(
                m_BossWidth, m_CurrentHeight, position, m_JumpVelocity, m_Gravity, moveDistance, m_IsOnPlatform);
            SetPosition(newPosition);
            position = newPosition;

            if (m_IsJumpingDown && position.y <= -51.0f) {
                m_IsJumpingDown = false;
                LOG_INFO("Boss has cleared platform range");
            }

            if (m_JumpVelocity <= 0.0f) {
                if (position.y <= -310.0f + m_CurrentHeight / 2) { // 地面底部 -310
                    SetPosition({position.x, -310.0f + m_CurrentHeight / 2});
                    m_JumpVelocity = 0.0f;
                    m_IsOnPlatform = false; // 地面不是平台
                    m_CurrentState = BossState::Stand;
                    SwitchAnimation(BossState::Stand);
                    LOG_INFO("Boss landed on ground");
                } else if (!m_IsJumpingDown && position.y <= -50.0f + m_CurrentHeight / 2 && position.y > -51.0f) { // 平台
                    SetPosition({position.x, -50.0f + m_CurrentHeight / 2});
                    m_JumpVelocity = 0.0f;
                    m_IsOnPlatform = true;
                    m_CurrentState = BossState::Stand;
                    SwitchAnimation(BossState::Stand);
                    LOG_INFO("Boss landed on platform");
                }
            }

            if (m_Health <= 0) {
                m_CurrentState = BossState::Lean;
                SwitchAnimation(BossState::Lean);
                m_DieTimer = 0.0f;
            }
        }
    } else if (m_CurrentState == BossState::Lean || m_CurrentState == BossState::Down || m_CurrentState == BossState::Die) {
        m_DieTimer += deltaTime;
        auto anim = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
        if ((anim && anim->GetState() == Util::Animation::State::ENDED) || m_DieTimer >= m_DieDuration) {
            m_DieTimer = 0.0f;
            if (m_CurrentState == BossState::Lean) {
                m_CurrentState = BossState::Down;
                SwitchAnimation(BossState::Down);
            } else if (m_CurrentState == BossState::Down) {
                m_CurrentState = BossState::Die;
                SwitchAnimation(BossState::Die);
            } else if (m_CurrentState == BossState::Die) {
                m_CurrentState = BossState::Dead;
                m_State = EnemyState::Dead;
                LOG_INFO("Boss defeated!");
            }
        }
    }
}

void Boss::OnHit() {
    if (m_CurrentState != BossState::Stand && m_CurrentState != BossState::Jump) return;
    m_Health--;
    LOG_INFO("Boss hit by bullet, Health: {}/{}", m_Health, m_MaxHealth);
}

void Boss::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (m_CurrentState == BossState::Dead) return;
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_CurrentState == BossState::Stand || m_CurrentState == BossState::Jump) {
            nick->Die();
        }
    }
}