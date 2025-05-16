#ifndef SNOWBALL_HPP
#define SNOWBALL_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"
#include "Util/SFX.hpp"

class Enemy;

class Snowball : public UpdatableDrawable , public std::enable_shared_from_this<Snowball>{
public:
    enum class SnowballState { Static, Pushed, Kicked, Killed };

    // 更新構造函數，接受來源敵人
    Snowball(const glm::vec2& initialPosition, std::shared_ptr<Enemy> sourceEnemy = nullptr);
    void Update() override;
    void OnHit();
    void OnKick(Direction direction);
    void SetMeltStage(int stage);
    void CheckCollisionWithEnemies();

    [[nodiscard]] bool IsMelted() const { return m_MeltStage == 3 && m_MeltTimer >= m_MeltDuration; }
    [[nodiscard]] int GetMeltStage() const { return m_MeltStage; }
    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] float GetWidth() const override { return m_Width; }
    [[nodiscard]] float GetHeight() const override { return m_Height; }
    [[nodiscard]] SnowballState GetSnowballState() const { return m_SnowballState; }
    [[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }

private:
    // State and Physics
    SnowballState m_SnowballState = SnowballState::Static;
    float m_MeltTimer = 0.0f;
    int   m_MeltStage = 3;
    bool  m_IsOnPlatform = false;
    float m_JumpVelocity = 0.0f;
    float m_RollingSpeed = 400.0f;
    Direction m_Direction = Direction::Right;

    // Constants
    float m_MeltDuration = 3.0f;
    const float m_Width = 42.0f;
    const float m_Height = 44.0f;
    float m_RollingTimer = 0.0f;
    const float m_RollingDuration = 6.0f;

    // Animations
    std::map<std::string, std::shared_ptr<Util::Animation>> m_Animations;

    // SFX
    std::shared_ptr<Util::SFX> m_KillEnemySFX;

    // 來源敵人
    std::weak_ptr<Enemy> m_SourceEnemy; // 使用 weak_ptr 避免循環引用

    // Private Methods
    void LoadAnimations();
    glm::vec2 CalculatePosition(float width, float height, float moveDistance = 0.0f);
};

#endif // SNOWBALL_HPP