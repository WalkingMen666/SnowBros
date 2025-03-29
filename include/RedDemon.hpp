#ifndef RED_DEMON_HPP
#define RED_DEMON_HPP

#include "Enemy.hpp"
#include "Util/Time.hpp"
#include "GameWorld.hpp"
#include "Snowball.hpp"
#include <memory>

class RedDemon : public Enemy, public std::enable_shared_from_this<RedDemon> {
public:
    enum class State { STAND, WALK, JUMP, FALL, DIE };

    RedDemon(const glm::vec2& pos);
    void Update() override;
    void OnHit() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;

    [[nodiscard]] float GetCharacterWidth() const override { return GetSizeForMeltStage().first; }
    [[nodiscard]] float GetCharacterHeight() const override { return GetSizeForMeltStage().second; }
    [[nodiscard]] float GetJumpVelocity() const override { return m_JumpVelocity; }
    [[nodiscard]] std::shared_ptr<Snowball> GetSnowball() const override { return m_Snowball; }

protected:
    // Movement and Physics
    float m_speed = 180.0f;
    float m_JumpVelocity = 0.0f;

    // Resources
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";
    std::shared_ptr<Snowball> m_Snowball;

    // Constants
    const float m_JumpInitialVelocity = 450.0f;
    const float m_Gravity = -800.0f;

private:
    // State and Action
    float m_ActionTimer = 0.0f;
    bool  m_IsActing = false;
    bool  m_IsChangingDirection = false;
    Direction m_TargetDirection;
    bool  m_IsOnPlatform = false;
    State m_CurrentState = State::STAND;

    // Constants
    const float ACTION_DELAY = 0.5f;

    // Private Methods
    void LoadAnimations();
    void SetState(State state);
    std::pair<float, float> GetSizeForMeltStage() const;
};

#endif // RED_DEMON_HPP