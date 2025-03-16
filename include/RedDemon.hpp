#ifndef RED_DEMON_HPP
#define RED_DEMON_HPP

#include "Enemy.hpp"
#include "Util/Time.hpp"

class RedDemon : public Enemy {
private:
    float m_speed = 50.0f;
    float m_JumpVelocity = 0.0f;
    const float m_JumpInitialVelocity = 450.0f;
    const float m_Gravity = -800.0f;
    float m_JumpTimer;
    const float JUMP_INTERVAL = 2.0f;
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";

public:
    RedDemon(const glm::vec2& pos);
    void Update() override;
    void JumpToPlatform();
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
private:
    void LoadAnimations();
};

#endif // RED_DEMON_HPP