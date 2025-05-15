#include "NickBullet.hpp"
#include "Util/Logger.hpp"

const std::string NICK_BULLET_PATH = RESOURCE_DIR "/Image/Character/Tom&Nick/";

NickBullet::NickBullet(const glm::vec2& pos, Direction dir)
    : Bullet(pos), m_Direction(dir) {
    SetVisible(true);
    SetZIndex(5);

    try {
        m_Velocity = (dir == Direction::Right) ? glm::vec2(800.0f, 0) : glm::vec2(-800.0f, 0);
        m_Drawable = std::make_shared<Util::Image>((dir == Direction::Right) ? NICK_BULLET_PATH + "blue_bullet_right.png" : NICK_BULLET_PATH + "blue_bullet_left.png");
        if (!m_Drawable) {
            LOG_ERROR("Failed to load bullet image");
            m_MarkedForRemoval = true;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Exception while loading bullet image: {}", e.what());
        m_MarkedForRemoval = true;
    }
}

void NickBullet::Update() {
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_LifeTime += deltaTime;
    m_Velocity.x *= 0.95f;
    if (m_LifeTime > 0.5f) m_Velocity.y -= 150.0f * deltaTime;
    m_Transform.translation += m_Velocity * deltaTime;

    if (m_LifeTime > MAX_LIFE || GetPosition().y < -380 || GetPosition().x > 410 || GetPosition().x < -410) {
        m_MarkedForRemoval = true;
    }
}

void NickBullet::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (const auto enemy = std::dynamic_pointer_cast<Enemy>(other)) {
        enemy->OnHit();
        m_MarkedForRemoval = true;
    }
} 