//
// Created by 20210602 on 2025/4/29/星期二.
//

#ifndef SMALLBOSS2_HPP
#define SMALLBOSS2_HPP


#include "Enemy.hpp"
#include "Util/Time.hpp"
#include "GameWorld.hpp"
#include "Snowball.hpp"
#include <memory>

class SmallBoss2 : public Enemy, public std::enable_shared_from_this<SmallBoss2> {
public:
    enum class State { STAND, WALK, FALL, DIE };

    SmallBoss2(const glm::vec2& pos);
    void Update() override;
    void OnHit() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
    void Die() override;

    [[nodiscard]] float GetCharacterWidth() const override { return GetSizeForMeltStage().first; }
    [[nodiscard]] float GetCharacterHeight() const override { return GetSizeForMeltStage().second; }
    [[nodiscard]] float GetJumpVelocity() const override { return m_JumpVelocity; }
    [[nodiscard]] std::shared_ptr<Snowball> GetSnowball() const override { return m_Snowball; }

protected:
    // Movement and Physics
    float m_speed = 180.0f;
    float m_JumpVelocity = 0.0f;
    float m_InitialVelocityX = 0.0f; // 初始水平速度
    bool m_HasInitialVelocity = false; // 是否使用初始速度

    // Ejection Phase
    bool m_IsEjecting = true; // 是否處於彈射階段

    // Resources
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Boss/";
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

    // Death Physics
    float m_DeathVelocity = 0.0f;
    float m_DeathTimer = 0.0f;
    bool  m_HasLanded = false;
    const float m_DeathDuration = 1.0f;
    const float m_LandingDuration = 0.4f;

    // Constants
    const float ACTION_DELAY = 0.5f;
    const float m_Width = 55.0f;
    const float m_Height = 55.0f;

    // Private Methods
    void LoadAnimations();
    void SetState(State state);
    std::pair<float, float> GetSizeForMeltStage() const;
};

#endif //SMALLBOSS2_HPP
