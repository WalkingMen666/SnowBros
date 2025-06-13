#include "Boss2.hpp"
#include "SmallBoss2.hpp"
#include "Nick.hpp"
#include "App.hpp"
#include <glm/glm.hpp>
#include <memory>

Boss2::Boss2(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Boss/boss2_5.png", pos) {
    LoadAnimations();
    m_Drawable = m_Animations["stand"];
    m_CurrentState = BossState::Stand;
    m_MaxHealth= 200;

}

void Boss2::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    // Handle Dead state
    if (m_CurrentState == BossState::Dead) {
        m_DieTimer += deltaTime;
        if (m_DieTimer >= m_DieDuration) {
            App::GetInstance().AddRemovingObject(shared_from_this());
        }
        return;
    }

    // Handle Disappear state (before position switch)
    if (m_CurrentState == BossState::Disappear) {
        static float disappearTimer = 0.0f;
        static const float disappearDuration = 1.0f; // 5 frames * 200ms = 1 second
        disappearTimer += deltaTime;

        if (disappearTimer >= disappearDuration) {
            // Switch position
            if (position.y == 90.0f) {
                position = glm::vec2(0.0f, -232.0f); // boss2地面
            } else {
                position = glm::vec2(0.0f, 90.0f);
            }
            SetPosition(position);

            // Transition to Appear state
            m_CurrentState = BossState::Appear;
            SwitchAnimation(BossState::Appear);
            disappearTimer = 0.0f;
        }
        return;
    }

    // Handle Appear state (after position switch)
    if (m_CurrentState == BossState::Appear) {
        static float appearTimer = 0.0f;
        static const float appearDuration = 1.0f; // 5 frames * 200ms = 1 second
        appearTimer += deltaTime;

        if (appearTimer >= appearDuration) {
            m_CurrentState = BossState::Shoot;
            SwitchAnimation(BossState::Shoot);
            appearTimer = 0.0f;

            // Spawn SmallBoss2 at four corners
            m_SmallBosses.clear();
            glm::vec2 corners[4] = {
                {position.x - 89.5f, position.y + 102}, // Top-left
                {position.x + 89.5f, position.y + 102}, // Top-right
                {position.x - m_BossWidth / 2, position.y - 18}, // Bottom-left
                {position.x + m_BossWidth / 2, position.y - 18}  // Bottom-right
            };

            for (int i = 0; i < 4; ++i) {
                auto smallBoss2 = std::make_shared<SmallBoss2>(corners[i]);
                if (corners[i].x < 0) {
                    smallBoss2->SetDirection(Direction::Left);
                    smallBoss2->SetInitialVelocityX(-200);
                } else {
                    smallBoss2->SetDirection(Direction::Right);
                    smallBoss2->SetInitialVelocityX(200);
                }
                
                // Add Y velocity for the first two corners (top corners)
                if (i < 2) {
                    smallBoss2->SetInitialVelocityY(700);
                }
                
                App::GetInstance().AddPendingObject(smallBoss2);
                m_SmallBosses.push_back(smallBoss2);
            }
        }
        return;
    }

    // Handle movement and state transitions for Stand and Shoot
    static float moveTimer = 0.0f;
    static const float moveInterval = 5.0f; // Time before disappearing
    moveTimer += deltaTime;

    if (moveTimer >= moveInterval && m_CurrentState == BossState::Stand) {
        // Start Disappear animation
        m_CurrentState = BossState::Disappear;
        SwitchAnimation(BossState::Disappear);
        moveTimer = 0.0f;
    } else if (m_CurrentState == BossState::Shoot) {
        // Stay in Shoot state for a short duration
        static float shootTimer = 0.0f;
        static const float shootDuration = 1.0f; // Duration of Shoot state
        shootTimer += deltaTime;

        if (shootTimer >= shootDuration) {
            // Transition back to Stand state
            m_CurrentState = BossState::Stand;
            SwitchAnimation(BossState::Stand);
            shootTimer = 0.0f;
            moveTimer = 0.0f;
        }
    }

    // Apply gravity if not on platform
    if (!m_IsOnPlatform && m_CurrentState != BossState::Appear && m_CurrentState != BossState::Disappear) {
        m_JumpVelocity += m_Gravity * deltaTime;
        position.y += m_JumpVelocity * deltaTime;
        position = GameWorld::map_collision_judgement(m_BossWidth, m_CurrentHeight, position, m_JumpVelocity, m_Gravity, 0.0f, m_IsOnPlatform);
        SetPosition(position);
    }
    float const BossTop = position.y + m_CurrentHeight / 2;
    float const BossBottom = position.y - m_CurrentHeight / 2;
    if (auto nick = App::GetInstance().GetNick()) {
        if ( glm::distance(GetPosition(), nick->GetPosition()) < (nick->GetCharacterWidth() + GetCharacterWidth() ) / 2 &&
             nick->GetPosition().y < BossTop && nick->GetPosition().y > BossBottom) {
            OnCollision(nick);
        }
    }
}

void Boss2::OnHit() {
    if (m_CurrentState == BossState::Dead) {
        return;
    }

    m_MaxHealth--;
    if (m_MaxHealth <= 0) {
        m_CurrentState = BossState::Dead;
        SwitchAnimation(BossState::Dead);
        m_DieTimer = 0.0f;
        // Remove SmallBoss2 instances
        for (auto& smallBoss : m_SmallBosses) {
            if (smallBoss) {
                App::GetInstance().AddRemovingObject(smallBoss);
            }
        }
        m_SmallBosses.clear();
    }
}

void Boss2::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_CurrentState != BossState::Dead) {
            nick->Die();
        }
    }
}

void Boss2::LoadAnimations() {
    // Appear animation (5 frames, during position switch)
    m_Animations["appear"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            RESOURCE_DIR "/Image/Character/Boss/boss2_1.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_2.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_3.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_4.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_5.png"
        }, false, 195, false, 0);

    // Stand animation
    m_Animations["stand"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{RESOURCE_DIR "/Image/Character/Boss/boss2_5.png"}, false, 200, false, 0);

    // Shoot animation
    m_Animations["shoot"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{RESOURCE_DIR "/Image/Character/Boss/boss2_6.png"}, false, 200, false, 0);

    // Dead animation (5 frames, assuming additional images)
    m_Animations["dead"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            RESOURCE_DIR "/Image/Character/Boss/boss2_die1.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_die2.png"
        }, false, 200, true, 0);

    // Disappear animation (5 frames, during position switch)
    m_Animations["disappear"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            RESOURCE_DIR "/Image/Character/Boss/boss2_5.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_4.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_3.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_2.png",
            RESOURCE_DIR "/Image/Character/Boss/boss2_1.png"
        }, false, 195, false, 0);
}

void Boss2::SwitchAnimation(BossState state) {
    switch (state) {
        case BossState::Appear:
            m_Drawable = m_Animations["appear"];
            m_Animations["appear"]->Play();
            break;
        case BossState::Stand:
            m_Drawable = m_Animations["stand"];
            m_Animations["stand"]->Play();
            break;
        case BossState::Shoot:
            m_Drawable = m_Animations["shoot"];
            m_Animations["shoot"]->Play();
            break;
        case BossState::Dead:
            m_Drawable = m_Animations["dead"];
            m_Animations["dead"]->Play();
            break;
        case BossState::Disappear:
            m_Drawable = m_Animations["disappear"];
            m_Animations["disappear"]->Play();
            break;
    }
}