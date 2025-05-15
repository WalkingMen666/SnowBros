#ifndef FAT_BULLET_HPP
#define FAT_BULLET_HPP

#include "Bullet.hpp"
#include "Nick.hpp"

class FatBullet : public Bullet {
public:
    FatBullet(const glm::vec2& pos, float angleDegrees);
    void Update() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;

private:
    float m_Angle;
    static constexpr float m_BulletWidth = 17.0f;
    static constexpr float m_BulletHeight = 17.0f;
};

#endif // FAT_BULLET_HPP