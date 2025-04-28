#ifndef BOSS_HPP
#define BOSS_HPP

#include "Enemy.hpp"
#include "Snowball.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"
#include <cstdlib>
#include <ctime>

class Boss : public Enemy {
public:
    enum class BossState { Stand, Jump, Lean, Down, Die, Dead };

    Boss(const glm::vec2& pos = {385.0f, 92.0f});
    void Update() override;
    void OnHit() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
    bool IsBoss() const override { return true; }
    float GetCharacterWidth() const override { return m_BossWidth; }
    float GetCharacterHeight() const override { return m_CurrentHeight; }
    float GetJumpVelocity() const override { return m_JumpVelocity; }
    int GetHealth() const { return m_Health; }
    int GetMaxHealth() const { return m_MaxHealth; }
    BossState GetState() const { return m_CurrentState; }


private:
    void LoadAnimations();
    void SwitchAnimation(BossState state);

    static constexpr float m_BossWidth = 224.0f;
    static constexpr float m_BossHeight = 284.0f;
    static constexpr float m_JumpInitialVelocity = 750.0f;
    static constexpr float m_Gravity = -800.0f;
    float m_JumpVelocity = 0.0f;
    bool m_IsOnPlatform = true;
    bool m_IsJumpingDown = false;
    float m_SmallJumpTimer = 0.0f;
    static constexpr float m_SmallJumpInterval = 2.5f;
    int m_SmallJumpCount = 0;
    int m_RandomJumpLimit = 5;
    int m_Health = 10;
    static constexpr int m_MaxHealth = 10;
    float m_DieTimer = 0.0f;
    static constexpr float m_DieDuration = 1.0f;
    BossState m_CurrentState = BossState::Stand;
    float m_CurrentHeight = m_BossHeight;
};

#endif // BOSS_HPP