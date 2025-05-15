#include "FrogBullet.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"

FrogBullet::FrogBullet(const glm::vec2& pos, Direction dir)
    : Bullet(pos), m_Direction(dir) {
    m_Transform.translation = pos;
    SetVisible(true); // 明確設置可見性
    SetZIndex(5); // 設置合理的 Z 軸，確保不被其他物件覆蓋

    try {
        switch (dir) {
            case Direction::Up:
                m_Velocity = glm::vec2(0, 220.0f);
                m_Drawable = std::make_shared<Util::Image>(BASE_PATH + "fire_up.png");
                break;
            case Direction::Down:
                m_Velocity = glm::vec2(0, -220.0f);
                m_Drawable = std::make_shared<Util::Image>(BASE_PATH + "fire_down.png");
                break;
            case Direction::Left:
                m_Velocity = glm::vec2(-220.0f, 0);
                m_Drawable = std::make_shared<Util::Image>(BASE_PATH + "fire_left.png");
                break;
            case Direction::Right:
                m_Velocity = glm::vec2(220.0f, 0);
                m_Drawable = std::make_shared<Util::Image>(BASE_PATH + "fire_right.png");
                break;
        }
        if (!m_Drawable) {
            LOG_ERROR("Failed to load bullet image for direction: {}", static_cast<int>(dir));
            m_MarkedForRemoval = true; // 如果圖片加載失敗，標記為移除
            return;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Exception while loading bullet image: {}", e.what());
        m_MarkedForRemoval = true;
        return;
    }

    try {
        m_ExplosionAnimation = std::make_shared<Util::Animation>(
            std::vector<std::string>{BASE_PATH + "boom.png"}, false, 500, false, 0);
        if (!m_ExplosionAnimation) {
            LOG_ERROR("Failed to load explosion animation");
            m_MarkedForRemoval = true;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Exception while loading explosion animation: {}", e.what());
        m_MarkedForRemoval = true;
    }
}

void FrogBullet::Update() {
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_LifeTime += deltaTime;

    if (m_IsExploding) {
        m_ExplosionTimer += deltaTime;
        if (m_ExplosionTimer >= m_ExplosionDuration) {
            m_MarkedForRemoval = true;
        }
    } else {
        m_Transform.translation += m_Velocity * deltaTime;
        if (m_LifeTime > MAX_LIFE || GetPosition().y < -380 || GetPosition().y > 330) {
            m_MarkedForRemoval = true;
        }
    }
}

void FrogBullet::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (m_IsExploding) return;
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        m_IsExploding = true;
        m_Drawable = m_ExplosionAnimation;
        if (m_ExplosionAnimation) {
            m_ExplosionAnimation->Play();
        } else {
            LOG_ERROR("Explosion animation is null during collision");
            m_MarkedForRemoval = true;
        }
        m_Velocity = glm::vec2(0, 0); // 停止移動
        nick->Die();
    }
}