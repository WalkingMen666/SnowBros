#ifndef BOSS2_HPP
#define BOSS2_HPP

#include "Enemy.hpp"
#include "Snowball.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"
#include "SmallBoss2.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <memory>

class Boss2 : public Enemy, public std::enable_shared_from_this<Boss2> {
public:
    enum class BossState { Stand, Shoot, Dead, Appear, Disappear };

    Boss2(const glm::vec2& pos = {0.0f, 200.0f} );
    void Update() override;
    void OnHit() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
    bool IsBoss() const override { return true; }
    float GetCharacterWidth() const override { return m_BossWidth; }
    float GetCharacterHeight() const override { return m_CurrentHeight; }
    float GetJumpVelocity() const override { return m_JumpVelocity; }
    // int GetHealth() const { return m_Health; }
    int GetMaxHealth() const { return m_MaxHealth; }
    BossState GetState() const { return m_CurrentState; }

private:
    void LoadAnimations();
    void SwitchAnimation(BossState state);

    static constexpr float m_BossWidth = 409.0f;
    static constexpr float m_BossHeight = 204.0f;
    // static constexpr float m_JumpInitialVelocity = 750.0f;
    static constexpr float m_Gravity = -800.0f;
    float m_JumpVelocity = 0.0f;
    bool m_IsOnPlatform = true;
    // int m_Health = 10;
    // static constexpr int m_MaxHealth = 10;
    float m_DieTimer = 0.0f;
    static constexpr float m_DieDuration = 1.0f;
    BossState m_CurrentState = BossState::Stand;
    float m_CurrentHeight = m_BossHeight;
    std::vector<std::shared_ptr<SmallBoss2>> m_SmallBosses; // Store SmallBoss2 instances
};

#endif //BOSS2_HPP