#include "Enemy.hpp"
#include "App.hpp"
#include "Bullet.hpp"
#include "Snowball.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"

Enemy::Enemy(const std::string& imagePath, const glm::vec2& pos)
    : Character(""), AnimatedCharacter(std::vector<std::string>{}), m_Velocity(0.0f, 0.0f), m_Direction(Direction::Right), m_HitCount(0),
      m_MaxHits(4),
      m_State(EnemyState::Normal)
{
    m_Transform.translation = pos;
    m_Drawable = std::make_shared<Util::Image>(imagePath);
}

void Enemy::OnHit(Bullet* bullet) {
    if (m_State == EnemyState::Normal) {
        m_HitCount++;
        if (m_HitCount >= m_MaxHits) {
            m_State = EnemyState::Snowball;
            SetAnimation("snowball_roll");
            GameWorld::AddObject(std::make_shared<Snowball>(m_Transform.translation));
            GameWorld::RemoveObject(std::shared_ptr<Enemy>(this));
        }
    }
}

void Enemy::SetAnimation(const std::string& key) {
    auto it = m_Animations.find(key);
    if (it != m_Animations.end()) {
        m_Drawable = it->second;
        std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->Play();
    }
}