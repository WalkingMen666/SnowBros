// Nick.hpp
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
        SPAWN,
        IDLE,
        WALK,
        ATTACK,
        JUMP,
        DIE
    };

    Nick();

    void Update();
    void Die();

    void SetState(State state);
    void SetInvincible(bool invincible);
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
    bool m_IsInvincible = false;
    float m_InvincibleTimer = 0.0f;
    const float m_InvincibleDuration = 2.0f;
    float m_BlinkTimer = 0.0f;
    const float m_BlinkInterval = 0.2f;

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
    float m_JumpVelocity = 0.0f;
    const float m_JumpInitialVelocity = 450.0f;
    const float m_Gravity = -800.0f;
    const float m_GroundLevel = -280.0f;
    static constexpr float TILE_SIZE = 5.0f; // 新瓦片大小
};

#endif // NICK_HPP