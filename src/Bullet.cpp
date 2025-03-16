#include "Bullet.hpp"
#include "Enemy.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"

Bullet::Bullet(const glm::vec2& pos, Direction dir) : m_LifeTime(0.0f) {
    m_Transform.translation = pos;
    m_Velocity = (dir == Direction::Right) ? glm::vec2(800.0f, 0) : glm::vec2(-800.0f, 0);
    m_Drawable = std::make_shared<Util::Image>((dir == Direction::Right) ? BASE_PATH + "blue_bullet_right.png" : BASE_PATH + "blue_bullet_left.png");
}

void Bullet::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_LifeTime += deltaTime;
    m_Velocity.x *= 0.95f;
    if (m_LifeTime > 0.5f) m_Velocity.y -= 150.0f * deltaTime;
    m_Transform.translation += m_Velocity * deltaTime;

    if (m_LifeTime > MAX_LIFE || GetPosition().y < -300) m_MarkedForRemoval = true;
}

void Bullet::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto enemy = std::dynamic_pointer_cast<Enemy>(other)) {
        enemy->OnHit(this);
        GameWorld::RemoveObject(std::shared_ptr<Bullet>(this));
    }
}