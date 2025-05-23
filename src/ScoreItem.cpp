#include "ScoreItem.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include <random>

ScoreItem::ScoreItem(ScoreType type, float x, float y)
    : m_Type(type)
    , m_IsActive(true)
    , m_Gravity(GRAVITY)
    , m_VelocityY(0.0f)
    , m_IsOnGround(false)
    , m_GroundY(0.0f)
    , m_LifeTime(0.0f)
{
    // 設置初始位置
    m_Transform.translation = glm::vec2(x, y);

    // 根據分數類型設置圖片
    std::string imagePath;
    switch (type) {
        case ScoreType::SCORE_100:
            imagePath = "Resources/Image/item/100pt.png";
            break;
        case ScoreType::SCORE_500:
            imagePath = "Resources/Image/item/500pt.png";
            break;
        case ScoreType::SCORE_1000:
            imagePath = "Resources/Image/item/1000pt.png";
            break;
        case ScoreType::SCORE_2000:
            imagePath = "Resources/Image/item/2000pt.png";
            break;
    }

    m_Drawable = std::make_shared<Util::Image>(imagePath);
}

void ScoreItem::Update() {
    if (!m_IsActive) return;

    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;  // 轉換為秒
    m_LifeTime += deltaTime;

    // 檢查生命週期
    if (m_LifeTime >= LIFE_DURATION) {
        m_IsActive = false;
        return;
    }

    // 如果還沒落地，進行自由落體運動
    if (!m_IsOnGround) {
        m_VelocityY += m_Gravity * deltaTime;
        m_Transform.translation.y += m_VelocityY * deltaTime;

        // 檢查是否到達預設的地面高度
        if (m_Transform.translation.y >= m_GroundY) {
            m_Transform.translation.y = m_GroundY;
            m_IsOnGround = true;
            m_VelocityY = 0.0f;
        }
    }
}

float ScoreItem::GetWidth() const {
    switch (m_Type) {
        case ScoreType::SCORE_100:
            return 15.0f;
        case ScoreType::SCORE_500:
            return 16.0f;
        case ScoreType::SCORE_1000:
            return 16.0f;
        case ScoreType::SCORE_2000:
            return 15.0f;
        default:
            return 0.0f;
    }
}

float ScoreItem::GetHeight() const {
    switch (m_Type) {
        case ScoreType::SCORE_100:
            return 16.0f;
        case ScoreType::SCORE_500:
            return 14.0f;
        case ScoreType::SCORE_1000:
            return 24.0f;
        case ScoreType::SCORE_2000:
            return 24.0f;
        default:
            return 0.0f;
    }
}

std::shared_ptr<Core::Drawable> ScoreItem::GetDrawable() const {
    return m_Drawable;
}

bool ScoreItem::IsCollidingWithNick(const glm::vec2& nickPos, float nickWidth, float nickHeight) const {
    if (!m_IsActive) return false;

    float itemLeft = m_Transform.translation.x - GetWidth() / 2;
    float itemRight = m_Transform.translation.x + GetWidth() / 2;
    float itemTop = m_Transform.translation.y - GetHeight() / 2;
    float itemBottom = m_Transform.translation.y + GetHeight() / 2;

    float nickLeft = nickPos.x - nickWidth / 2;
    float nickRight = nickPos.x + nickWidth / 2;
    float nickTop = nickPos.y - nickHeight / 2;
    float nickBottom = nickPos.y + nickHeight / 2;

    return !(itemRight < nickLeft || itemLeft > nickRight ||
             itemBottom < nickTop || itemTop > nickBottom);
}

int ScoreItem::GetScore() const {
    switch (m_Type) {
        case ScoreType::SCORE_100:
            return 100;
        case ScoreType::SCORE_500:
            return 500;
        case ScoreType::SCORE_1000:
            return 1000;
        case ScoreType::SCORE_2000:
            return 2000;
        default:
            return 0;
    }
} 