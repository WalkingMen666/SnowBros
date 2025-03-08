#ifndef NICK_HPP
#define NICK_HPP

#include <vector>
#include <string>
#include <memory>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"

class Nick : public Util::GameObject {
public:
    enum class State {
        SPAWN,       // 出生
        INVINCIBLE,  // 無敵狀態
        IDLE,        // 待機
        WALK,        // 走路
        ATTACK,      // 攻擊
        JUMP,        // 跳躍
        DIE,         // 死亡
        PUSH         // 推雪球
    };

    Nick();

    void Update();

    void SetState(State state);
    [[nodiscard]] State GetState() const { return m_State; }
    [[nodiscard]] bool IsAnimationFinished() const;
    void SetDirection(bool facingRight);

private:
    void LoadAnimations();
    void SwitchAnimation(State state, bool looping);

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
    std::shared_ptr<Util::Animation> m_PushLeftAnimation;
    std::shared_ptr<Util::Animation> m_PushRightAnimation;

    float m_Speed = 200.0f;
    glm::vec2 m_Position = {0.0f, 0.0f};
    float m_InvincibleTimer = 0.0f;
    const float m_InvincibleDuration = 3.0f;
    bool m_IsVisible = true;
    float m_BlinkTimer = 0.0f;
    const float m_BlinkInterval = 0.2f;
};

#endif // NICK_HPP