#ifndef NICK_HPP
#define NICK_HPP

#include <memory>

#include "Character.hpp"
#include "AnimatedCharacter.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"

class Nick : public Character, public AnimatedCharacter {
public:
    enum class State {
        SPAWN,      // 出生
        INVINCIBLE, // 無敵狀態
        IDLE,       // 待機
        WALK,       // 走路
        ATTACK,     // 攻擊
        JUMP,       // 跳躍
        DIE         // 死亡
    };

    Nick();

    void Update();

    void SetState(State state);
    [[nodiscard]] State GetState() const { return m_State; }
    [[nodiscard]] bool IsAnimationFinished() const { return IfAnimationEnds(); }
    void SetDirection(bool facingRight);

private:
    void LoadAnimations();
    void SwitchAnimation(State state, bool looping);
    void HandleMovement(float deltaTime, glm::vec2& position);

private:
    State m_State = State::SPAWN;
    bool m_FacingRight = true;

    std::shared_ptr<Util::Animation> m_SpawnAnimation;
    std::shared_ptr<Util::Animation> m_IdleLeftAnimation;
    std::shared_ptr<Util::Animation> m_IdleRightAnimation;
    std::shared_ptr<Util::Animation> m_WalkLeftAnimation;
    std::shared_ptr<Util::Animation> m_WalkRightAnimation;
    std::shared_ptr<Util::Animation> m_AttackLeftAnimation;
    std::shared_ptr<Util::Animation> m_AttackRightAnimation;
    std::shared_ptr<Util::Animation> m_JumpLeftAnimation;
    std::shared_ptr<Util::Animation> m_JumpRightAnimation;
    std::shared_ptr<Util::Animation> m_DieAnimation;

    float m_Speed = 150.0f;
    float m_InvincibleTimer = 0.0f;
    const float m_InvincibleDuration = 2.0f;
    float m_BlinkTimer = 0.0f;
    const float m_BlinkInterval = 0.2f;

    // 跳躍物理相關
    float m_JumpVelocity = 0.0f;    // 當前垂直速度
    const float m_JumpInitialVelocity = 400.0f; // 跳躍初速度
    const float m_Gravity = -800.0f; // 重力加速度
    const float m_GroundLevel = 0.0f; // 地面高度
};

#endif // NICK_HPP