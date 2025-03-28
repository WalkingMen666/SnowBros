#ifndef RED_DEMON_HPP
#define RED_DEMON_HPP

#include "Enemy.hpp"
#include "Util/Time.hpp"
#include "GameWorld.hpp"

class RedDemon : public Enemy {
public:
    enum class State { STAND, WALK, JUMP, FALL, DIE };

protected:
    float m_speed = 180.0f;
    float m_JumpVelocity = 0.0f; // 保留：Normal 狀態使用，Snowball 狀態固定為 0
    const float m_JumpInitialVelocity = 450.0f;
    const float m_Gravity = -800.0f; // 保留：Snowball 狀態固定使用 -800
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
    std::pair<float, float> GetSizeForMeltStage() const; // 修改：統一返回 42x44

public:
    RedDemon(const glm::vec2& pos);
    void Update() override;
    void OnHit(Bullet* bullet) override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
    float GetCharacterWidth() const override { return GetSizeForMeltStage().first; }
    float GetCharacterHeight() const override { return GetSizeForMeltStage().second; }
    float GetJumpVelocity() const override { return m_JumpVelocity; }
};

#endif // RED_DEMON_HPP