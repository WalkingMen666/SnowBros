#include "Enemy.hpp"
#include "App.hpp"
#include "Bullet.hpp"
#include "Snowball.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"
#include "Nick.hpp"

Enemy::Enemy(const std::string& imagePath, const glm::vec2& pos)
    : Character(""), AnimatedCharacter({}), m_Velocity(0.0f, 0.0f), m_Direction(Direction::Right),
      m_HitCount(0), m_MaxHits(1), m_State(EnemyState::Normal)
{
    m_Transform.translation = pos;
    m_Drawable = std::make_shared<Util::Image>(imagePath);
}

void Enemy::OnHit() {
    if (m_State == EnemyState::Normal) {
        m_HitCount++;
        LOG_INFO("Enemy hit, m_HitCount: {}", m_HitCount);
        if (m_HitCount >= m_MaxHits) {
            m_State = EnemyState::Snowball;
            SetMeltStage(3);
        }
    } else if (m_State == EnemyState::Snowball && m_MeltStage > 1) {
        SetMeltStage(m_MeltStage - 1);
    }
}

void Enemy::SetAnimation(const std::string& key) {
    auto it = m_Animations.find(key);
    if (it != m_Animations.end()) {
        m_Drawable = it->second;
        std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->Play();
    } else {
        LOG_ERROR("Animation key {} not found!", key);
    }
}

void Enemy::SetMeltStage(int stage) {
    m_MeltStage = stage;
    m_MeltTimer = 0.0f;
    switch (m_MeltStage) {
    case 1: SetAnimation("snowball_melt_1"); break;
    case 2: SetAnimation("snowball_melt_2"); break;
    case 3: SetAnimation("snowball_melt_3"); break;
    default: break;
    }
    LOG_INFO("Set melt stage to: {}", m_MeltStage);
}