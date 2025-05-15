#ifndef BULLET_HPP
#define BULLET_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include <glm/glm.hpp>
#include <memory>

class Bullet : public UpdatableDrawable {
public:
    Bullet(const glm::vec2& pos);
    virtual void Update() override = 0;
    virtual void OnCollision(std::shared_ptr<Util::GameObject> other) = 0;

    [[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool IsMarkedForRemoval() const { return m_MarkedForRemoval; }
    [[nodiscard]] float GetWidth() const override { return m_BulletWidth; }
    [[nodiscard]] float GetHeight() const override { return m_BulletHeight; }

protected:
    // Movement and State
    glm::vec2 m_Velocity;
    float m_LifeTime = 0.0f;
    bool m_MarkedForRemoval = false;

    // Constants
    static constexpr float MAX_LIFE = 2.0f;
    static constexpr float m_BulletWidth = 17.0f;
    static constexpr float m_BulletHeight = 17.0f;
    static const std::string BASE_PATH;
};

#endif // BULLET_HPP