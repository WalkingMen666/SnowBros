#include "Snowball.hpp"
#include "GameWorld.hpp"
#include "Nick.hpp"

Snowball::Snowball(const glm::vec2& initialPosition) {
    LoadAnimations();
    m_Drawable = m_Animations["snowball_melt_3"];
    m_Transform.translation = initialPosition;
    SetVisible(true);
}

void Snowball::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    float width = 42.0f;
    float height = 44.0f;
    glm::vec2 position = m_Transform.translation;

    bool isBeingPushed = false;
    float moveDistance = 0.0f;
    if (auto nick = App::GetInstance().GetNick()) {
        float nickSpeed = nick->GetSpeed();
        glm::vec2 nickPos = nick->GetPosition();
        float distance = glm::distance(position, nickPos);
        float collisionThreshold = (nick->GetCharacterWidth() + width) / 2;

        if (distance < collisionThreshold) {
            if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
                moveDistance = -nickSpeed * deltaTime;
                isBeingPushed = true;
            } else if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
                moveDistance = nickSpeed * deltaTime;
                isBeingPushed = true;
            }
        }

        if (isBeingPushed) {
            bool snowballOnPlatform = false;
            float dummyVelocity = 0.0f;
            glm::vec2 newPosition = GameWorld::map_collision_judgement(
                width, height, position, dummyVelocity, -800.0f, moveDistance, snowballOnPlatform);

            float nickRight = nickPos.x + nick->GetCharacterWidth() / 2;
            float nickLeft = nickPos.x - nick->GetCharacterWidth() / 2;
            float snowballRight = newPosition.x + width / 2;
            float snowballLeft = newPosition.x - width / 2;

            bool movingRight = moveDistance > 0;
            bool movingLeft = moveDistance < 0;
            bool nickOnLeft = nickPos.x < position.x;
            bool nickOnRight = nickPos.x > position.x;

            if ((movingRight && nickOnLeft) || (movingLeft && nickOnRight)) {
                position = newPosition;
                if (movingRight && nickRight > snowballLeft) {
                    nick->SetPosition({snowballLeft - nick->GetCharacterWidth() / 2, nickPos.y});
                } else if (movingLeft && nickLeft < snowballRight) {
                    nick->SetPosition({snowballRight + nick->GetCharacterWidth() / 2, nickPos.y});
                }
            } else {
                isBeingPushed = false;
            }
        }
    }

    if (!isBeingPushed) {
        position = CalculatePosition(width, height);
    }

    if (!isBeingPushed) {
        m_MeltTimer += deltaTime;
    }

    if (m_MeltTimer >= m_MeltDuration && m_MeltStage < 3) {
        m_MeltTimer = 0.0f;
        SetMeltStage(m_MeltStage + 1);
    }

    m_Transform.translation = position;
}

void Snowball::OnHit() {
    if (m_MeltStage > 1) {
        SetMeltStage(--m_MeltStage);
    } else if (m_MeltStage == 1) {
        m_MeltTimer = 0.0f;
    }
}

glm::vec2 Snowball::CalculatePosition(float width, float height) {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    float gravity = -800.0f;
    float moveDistance = 0.0f;

    m_IsOnPlatform ? m_JumpVelocity = 0.0f : m_JumpVelocity += gravity * deltaTime;
    return GameWorld::map_collision_judgement(width, height, m_Transform.translation, m_JumpVelocity, gravity, moveDistance, m_IsOnPlatform);
}

void Snowball::SetMeltStage(int stage) {
    m_MeltStage = stage;
    switch (stage) {
        case 1: m_Drawable = m_Animations["snowball_melt_1"]; break;
        case 2: m_Drawable = m_Animations["snowball_melt_2"]; break;
        case 3: m_Drawable = m_Animations["snowball_melt_3"]; break;
        default: m_Drawable = m_Animations["snowball_roll"]; break;
    }
}

void Snowball::LoadAnimations() {
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";
    m_Animations["snowball_roll"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_roll_1.png", BASE_PATH + "snowball_roll_2.png",
                                 BASE_PATH + "snowball_roll_3.png", BASE_PATH + "snowball_roll_4.png"},
        false, 200, true, 0);
    m_Animations["snowball_melt_1"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_melt_1.png"}, false, 500, true, 0);
    m_Animations["snowball_melt_2"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_melt_2.png"}, false, 500, true, 0);
    m_Animations["snowball_melt_3"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_melt_3.png"}, false, 500, true, 0);
}