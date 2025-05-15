#ifndef FROG_BULLET_HPP
#define FROG_BULLET_HPP

#include "Bullet.hpp"
#include "Nick.hpp"

class FrogBullet : public Bullet {
public:
    enum class Direction { Up, Down, Left, Right };

    FrogBullet(const glm::vec2& pos, Direction dir);
    void Update() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;

    [[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool IsMarkedForRemoval() const { return m_MarkedForRemoval; }
    [[nodiscard]] float GetWidth() const override { return m_BulletWidth; }
    [[nodiscard]] float GetHeight() const override { return m_BulletHeight; }

private:
    Direction m_Direction;
    bool m_IsExploding = false;
    float m_ExplosionTimer = 0.0f;
    const float m_ExplosionDuration = 0.5f;
    std::shared_ptr<Util::Animation> m_ExplosionAnimation;
    static constexpr float m_BulletWidth = 27.0f;
    static constexpr float m_BulletHeight = 27.0f;
};

#endif // FROG_BULLET_HPP