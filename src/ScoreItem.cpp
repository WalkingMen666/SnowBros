#include "ScoreItem.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include <random>
#include "GameWorld.hpp"

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
            imagePath = RESOURCE_DIR"/Image/item/100pt.png";
            break;
        case ScoreType::SCORE_500:
            imagePath = RESOURCE_DIR"/Image/item/500pt.png";
            break;
        case ScoreType::SCORE_1000:
            imagePath = RESOURCE_DIR"/Image/item/1000pt.png";
            break;
        case ScoreType::SCORE_2000:
            imagePath = RESOURCE_DIR"/Image/item/2000pt.png";
            break;
        default:
            LOG_INFO("Invalid ScoreType: " + std::to_string(static_cast<int>(type)));
        imagePath = RESOURCE_DIR"/Image/item/object_26.png"; // 預設圖片
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

    glm::vec2 position = m_Transform.translation;
    bool isOnPlatform = m_IsOnGround;
    m_VelocityY += m_Gravity * deltaTime; // 應用重力
    glm::vec2 newPosition = GameWorld::map_collision_judgement(
        GetWidth(), GetHeight(), position, m_VelocityY, m_Gravity, 0.0f, isOnPlatform);

    m_Transform.translation = newPosition;
    m_IsOnGround = isOnPlatform;
    if (m_IsOnGround) {
        m_VelocityY = 0.0f;
    }

    if (auto nick = App::GetInstance().GetNick()) {
        if (glm::distance(GetPosition(), nick->GetPosition()) < (nick->GetCharacterWidth() + GetWidth()) / 2) {
            OnCollision(nick);
            m_IsActive = false;
            App::GetInstance().AddRemovingObject(shared_from_this());
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

void ScoreItem::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        if (m_IsActive) {
            ScoreType scoreType = m_Type;
            switch (scoreType) {
                case ScoreType::SCORE_100:
                    nick->AddScore(100);
                    break;
                case ScoreType::SCORE_500:
                    nick->AddScore(500);
                break;
                case ScoreType::SCORE_1000:
                    nick->AddScore(1000);
                break;
                case ScoreType::SCORE_2000:
                    nick->AddScore(2000);
                break;
            }
        }
    }
}

// bool ScoreItem::IsCollidingWithNick(const glm::vec2& nickPos, float nickWidth, float nickHeight) const {
//     if (!m_IsActive) return false;
//
//     float itemLeft = m_Transform.translation.x - GetWidth() / 2;
//     float itemRight = m_Transform.translation.x + GetWidth() / 2;
//     float itemTop = m_Transform.translation.y - GetHeight() / 2;
//     float itemBottom = m_Transform.translation.y + GetHeight() / 2;
//
//     float nickLeft = nickPos.x - nickWidth / 2;
//     float nickRight = nickPos.x + nickWidth / 2;
//     float nickTop = nickPos.y - nickHeight / 2;
//     float nickBottom = nickPos.y + nickHeight / 2;
//
//     return !(itemRight < nickLeft || itemLeft > nickRight ||
//              itemBottom < nickTop || itemTop > nickBottom);
// }

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