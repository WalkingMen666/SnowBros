#ifndef SNOWBALL_HPP
#define SNOWBALL_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"

enum class SnowballState { Static, Rolling, Pushed };

class Snowball : public UpdatableDrawable {
private:
    float m_MeltTimer = 0.0f;
    float m_MeltDuration = 3.0f;
    int m_MeltStage = 3;
    bool m_IsOnPlatform = false;
    float m_JumpVelocity = 0.0f;
    std::map<std::string, std::shared_ptr<Util::Animation>> m_Animations;

    void LoadAnimations();
    glm::vec2 CalculatePosition(float width, float height);

public:
    Snowball(const glm::vec2& initialPosition);  // 添加初始位置參數
    void Update() override;
    void OnHit();  // 新增被攻擊處理函數
    bool IsMelted() const { return m_MeltStage == 3 && m_MeltTimer >= m_MeltDuration; }
    int GetMeltStage() const { return m_MeltStage; }
    void SetMeltStage(int stage);
    glm::vec2 GetPosition() const { return m_Transform.translation; }

    float GetWidth() const override { return 42.0f; }
    float GetHeight() const override { return 44.0f; }
    std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
};

#endif // SNOWBALL_HPP