#ifndef BULLET_HPP
#define BULLET_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"

class Bullet : public UpdatableDrawable {
public:
    Bullet(const glm::vec2& pos, Direction dir);
    void Update() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other);

    [[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool IsMarkedForRemoval() const { return m_MarkedForRemoval; }
    [[nodiscard]] float GetBulletWidth() const { return m_BulletWidth; }
    [[nodiscard]] float GetBulletHeight() const { return m_BulletHeight; }
    [[nodiscard]] float GetWidth() const override { return m_BulletWidth; }
    [[nodiscard]] float GetHeight() const override { return m_BulletHeight; }

private:
    // Movement and State
    glm::vec2 m_Velocity;
    float m_LifeTime;
    bool  m_MarkedForRemoval = false;

    // Constants
    static constexpr float MAX_LIFE = 2.0f;
    static const std::string BASE_PATH;
    static constexpr float m_BulletWidth = 13.0f;
    static constexpr float m_BulletHeight = 24.0f;
};

#endif // BULLET_HPP