#ifndef BULLET_HPP
#define BULLET_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"

class Bullet : public UpdatableDrawable {
private:
    glm::vec2 m_Velocity;
    float m_LifeTime;
    const float MAX_LIFE = 2.0f;
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Tom&Nick/";
    bool m_MarkedForRemoval = false;

public:
    Bullet(const glm::vec2& pos, Direction dir);
    void Update() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other);
    std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }
    bool IsMarkedForRemoval() const { return m_MarkedForRemoval; }
};

#endif // BULLET_HPP