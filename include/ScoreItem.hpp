#ifndef SCORE_ITEM_HPP
#define SCORE_ITEM_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Time.hpp"
#include <string>

class ScoreItem : public UpdatableDrawable {
public:
    enum class ScoreType {
        SCORE_100,
        SCORE_500,
        SCORE_1000,
        SCORE_2000
    };

    ScoreItem(ScoreType type, float x, float y);
    virtual ~ScoreItem() = default;

    void Update() override;
    float GetWidth() const override;
    float GetHeight() const override;
    std::shared_ptr<Core::Drawable> GetDrawable() const override;
    
    bool IsCollidingWithNick(const glm::vec2& nickPos, float nickWidth, float nickHeight) const;
    int GetScore() const;
    void SetActive(bool active) { m_IsActive = active; }
    bool IsActive() const { return m_IsActive; }
    void SetGroundY(float y) { m_GroundY = y; }

private:
    ScoreType m_Type;
    std::shared_ptr<Core::Drawable> m_Drawable;
    bool m_IsActive;
    float m_Gravity;
    float m_VelocityY;
    bool m_IsOnGround;
    float m_GroundY;
    float m_LifeTime;  // 改用float來累計時間
    static constexpr float LIFE_DURATION = 10.0f; // 10秒生命週期
    static constexpr float GRAVITY = 500.0f; // 重力加速度
};

#endif // SCORE_ITEM_HPP 