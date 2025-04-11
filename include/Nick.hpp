#ifndef NICK_HPP
#define NICK_HPP

#include "Character.hpp"
#include "AnimatedCharacter.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"
#include "UpdatableDrawable.hpp"
#include "GameWorld.hpp"
#include "Enemy.hpp"

class App;

class Nick : public UpdatableDrawable, public Character, public AnimatedCharacter {
public:
    enum class State { SPAWN, IDLE, WALK, ATTACK, JUMP, DIE, PUSH, KICK };

    Nick();
    void Update() override;
    void Die();
    void SetState(State state);
    void SetInvincible(bool invincible);
    void OnCollision(std::shared_ptr<Util::GameObject> other);
    void SetDirection(bool facingRight);

    [[nodiscard]] State GetState() const { return m_State; }
    [[nodiscard]] bool IsAnimationFinished() const { return IfAnimationEnds(); }
    [[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    [[nodiscard]] float GetCharacterWidth() const { return characterWidth; }
    [[nodiscard]] float GetWidth() const override { return characterWidth; }
    [[nodiscard]] float GetHeight() const override { return characterHeight; }
    [[nodiscard]] float GetSpeed() const { return m_Speed; }

private:
    // State and Animation
    State m_State = State::SPAWN;
    bool  m_FacingRight = true;

    // Physics
    bool  m_IsOnPlatform = false;
    float m_Speed = 150.0f;
    float m_JumpVelocity = 0.0f;
    float m_Gravity = -800.0f;
    float m_DeathVelocity = 0.0f;

    // Invincibility
    bool  m_IsInvincible = false;
    float m_InvincibleTimer = 0.0f;
    float m_BlinkTimer = 0.0f;

    // Life and Death
    int   m_Lives = 3;
    float m_DeathPauseTimer = 0.0f;

    // Constants
    static constexpr float characterWidth        = 35.0f;
    static constexpr float characterHeight       = 55.0f;
    static constexpr float m_JumpInitialVelocity = 450.0f;
    static constexpr float m_InvincibleDuration  = 4.0f;
    static constexpr float m_BlinkInterval       = 0.2f;
    static constexpr float m_DeathInitialVelocity= 100.0f;
    static constexpr float m_DeathPauseDuration  = 1.0f;

    // Animations
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
    std::shared_ptr<Util::Animation> m_PushLeftAnimation;
    std::shared_ptr<Util::Animation> m_PushRightAnimation;
    std::shared_ptr<Util::Animation> m_KickLeftAnimation;
    std::shared_ptr<Util::Animation> m_KickRightAnimation;

    // Private Methods
    void LoadAnimations();
    void SwitchAnimation(State state, bool looping);
};

#endif // NICK_HPP