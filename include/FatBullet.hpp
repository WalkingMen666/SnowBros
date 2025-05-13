#ifndef FAT_BULLET_HPP
#define FAT_BULLET_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include "Nick.hpp"
#include <glm/glm.hpp>
#include <memory>

class FatBullet : public UpdatableDrawable {
public:
    FatBullet(const glm::vec2& pos, float angleDegrees);

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

    // Constants
    static constexpr float MAX_LIFE = 2.0f;
    static constexpr float m_BulletWidth = 17.0f;
    static constexpr float m_BulletHeight = 17.0f;
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";
};

#endif // FAT_BULLET_HPP