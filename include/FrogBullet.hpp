#ifndef FROG_BULLET_HPP
#define FROG_BULLET_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include "Nick.hpp"

class FrogBullet : public UpdatableDrawable {
public:
    enum class Direction { Up, Down, Left, Right };

    FrogBullet(const glm::vec2& pos, Direction dir);
    void Update() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other);

    [[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool IsMarkedForRemoval() const { return m_MarkedForRemoval; }
    [[nodiscard]] float GetWidth() const override { return m_BulletWidth; }
    [[nodiscard]] float GetHeight() const override { return m_BulletHeight; }

private:
    // Movement and State
    glm::vec2 m_Velocity;
    float m_LifeTime = 0.0f;
    bool m_MarkedForRemoval = false;
    bool m_IsExploding = false;
    float m_ExplosionTimer = 0.0f;
    const float m_ExplosionDuration = 0.5f; // 爆炸動畫持續時間

    // Drawable
    std::shared_ptr<Util::Animation> m_ExplosionAnimation;

    // Constants
    static constexpr float MAX_LIFE = 2.0f;
    static constexpr float m_BulletWidth = 46.0f;
    static constexpr float m_BulletHeight = 31.0f;
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";
};

#endif // FROG_BULLET_HPP