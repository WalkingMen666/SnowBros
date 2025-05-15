#ifndef NICK_BULLET_HPP
#define NICK_BULLET_HPP

#include "Bullet.hpp"
#include "Enemy.hpp"

class NickBullet : public Bullet {
public:
    enum class Direction { Left, Right };

    NickBullet(const glm::vec2& pos, Direction dir);
    void Update() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;

private:
    Direction m_Direction;
    static constexpr float m_BulletWidth = 13.0f;
    static constexpr float m_BulletHeight = 24.0f;
};

#endif // NICK_BULLET_HPP 