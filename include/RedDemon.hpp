#ifndef RED_DEMON_HPP
#define RED_DEMON_HPP

#include "Enemy.hpp"
#include "Util/Time.hpp"
#include "GameWorld.hpp"

class RedDemon : public Enemy {
public:
    enum class State { STAND, WALK, JUMP, FALL, DIE };

protected: // 改為 protected 以便基類訪問
    float m_speed = 180.0f;
    float m_JumpVelocity = 0.0f;
    const float m_JumpInitialVelocity = 450.0f;
    const float m_Gravity = -800.0f;
    const float characterWidth = 46.0f;
    const float characterHeight = 46.0f;
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";

private:
    float m_ActionTimer = 0.0f;
    const float ACTION_DELAY = 0.5f;
    bool m_IsActing = false;
    bool m_IsChangingDirection = false;
    Direction m_TargetDirection;
    bool m_IsOnPlatform = false;
    State m_CurrentState = State::STAND;

    void LoadAnimations();
    void SetState(State state);

public:
    RedDemon(const glm::vec2& pos);
    void Update() override;
    void OnHit(Bullet* bullet) override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
    float GetCharacterWidth() const override { return characterWidth; } // 實現 GetCharacterWidth
};

#endif // RED_DEMON_HPP