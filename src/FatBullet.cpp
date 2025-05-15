#include "FatBullet.hpp"
#include "Util/Logger.hpp"
#include <glm/gtc/constants.hpp>

FatBullet::FatBullet(const glm::vec2& pos, float angleDegrees) 
    : Bullet(pos), m_Angle(angleDegrees) {
    SetVisible(true);
    SetZIndex(5);

    try {
        // Convert angle to radians
        float angleRadians = angleDegrees * glm::pi<float>() / 180.0f;
        // Set velocity based on angle (speed = 220.0f, same as FrogBullet)
        m_Velocity = glm::vec2(220.0f * std::cos(angleRadians), 220.0f * std::sin(angleRadians));
        m_Drawable = std::make_shared<Util::Image>(BASE_PATH + "fat_bullet.png");
        if (!m_Drawable) {
            LOG_ERROR("Failed to load fat bullet image");
            m_MarkedForRemoval = true;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Exception while loading fat bullet image: {}", e.what());
        m_MarkedForRemoval = true;
    }
}

void FatBullet::Update() {
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_LifeTime += deltaTime;

    m_Transform.translation += m_Velocity * deltaTime;
    if (m_LifeTime > MAX_LIFE || GetPosition().y < -380 || GetPosition().x > 410 || GetPosition().x < -410) {
        m_MarkedForRemoval = true;
    }
}

void FatBullet::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        nick->Die();
        m_MarkedForRemoval = true;
    }
}