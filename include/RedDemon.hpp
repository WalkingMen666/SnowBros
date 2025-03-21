#ifndef RED_DEMON_HPP
#define RED_DEMON_HPP

#include "Enemy.hpp"
#include "Util/Time.hpp"
#include "GameWorld.hpp"

class RedDemon : public Enemy {
public:
    enum class State { STAND, WALK, JUMP, FALL, DIE }; // 新增狀態機

private:
    float m_speed = 180.0f;
    float m_JumpVelocity = 0.0f;
    const float m_JumpInitialVelocity = 450.0f;
    const float m_Gravity = -800.0f;
    const float characterWidth = 46.0f;
    const float characterHeight = 46.0f;
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";

    // 控制停滯和動作切換
    float m_ActionTimer = 0.0f;          // 計時器，用於控制停滯時間
    const float ACTION_DELAY = 0.5f;     // 停滯時間（0.5秒）
    bool m_IsActing = false;             // 是否正在執行動作（跳躍或下降）
    bool m_IsChangingDirection = false;  // 是否正在改變方向
    Direction m_TargetDirection;         // 目標方向
    bool m_IsOnPlatform = false;         // 是否站在平台上（非地面）
    State m_CurrentState = State::STAND; // 當前狀態，預設為 STAND

    void LoadAnimations();
    void SetState(State state);          // 設置狀態並更新動畫

public:
    RedDemon(const glm::vec2& pos);
    void Update() override;
    void OnCollision(std::shared_ptr<Util::GameObject> other) override;
};

#endif // RED_DEMON_HPP