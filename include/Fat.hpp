#ifndef FAT_HPP
#define FAT_HPP

#include "Enemy.hpp"
#include "Util/Time.hpp"
#include "GameWorld.hpp"
#include "Snowball.hpp"
#include <memory>

class Fat : public Enemy, public std::enable_shared_from_this<Fat> {
public:
    enum class State { STAND, WALK, DIE, ON_EDGE, ATTACK};

    Fat(const glm::vec2& pos);
    void Update() override;
    void OnHit() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
    void Die() override;

    [[nodiscard]] float GetCharacterWidth() const override { return m_Width; }
    [[nodiscard]] float GetCharacterHeight() const override { return m_Height; }
    [[nodiscard]] float GetJumpVelocity() const override { return m_JumpVelocity; }
    [[nodiscard]] std::shared_ptr<Snowball> GetSnowball() const override { return m_Snowball; }

protected:
    // Movement and Physics
    float m_speed = 100.0f;
    float m_JumpVelocity = 0.0f;

    // Resources
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";
    std::shared_ptr<Snowball> m_Snowball;

    // Constants
    const float m_Gravity = -800.0f;

private:
    // State and Action
    float m_ActionTimer = 0.0f;
    bool  m_IsChangingDirection = false;
    Direction m_TargetDirection;
    bool  m_IsOnPlatform = false;
    State m_CurrentState = State::STAND;
    bool  m_hasAttacked = false;

    // Death Blink Animation
    float m_TotalBlinkTimer = 0.0f;     // 總閃爍時間計時器
    float m_IntervalBlinkTimer = 0.0f;  // 閃爍間隔計時器
    float m_BlinkInterval = 0.2f;       // 初始閃爍間隔
    const float m_BlinkDuration = 1.5f; // 閃爍總持續時間
    bool  m_IsVisible = true;           // 當前是否可見
    const float m_MinBlinkInterval = 0.05f; // 最小閃爍間隔

    // Constants
    const float ACTION_DELAY = 1.5f;
    const float m_Width = 62.0f;
    const float m_Height = 62.0f;

    // Private Methods
    void LoadAnimations();
    void SetState(State state);
    void Attack();
};

#endif