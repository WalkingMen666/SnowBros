#include "Snowball.hpp"
#include "GameWorld.hpp"
#include "Nick.hpp"
#include "Enemy.hpp"

Snowball::Snowball(const glm::vec2& initialPosition, std::shared_ptr<Enemy> sourceEnemy)
    : m_SourceEnemy(sourceEnemy) {
    LoadAnimations();
    m_Drawable = m_Animations["snowball_melt_3"];
    m_Transform.translation = initialPosition;
    SetVisible(true);
}

void Snowball::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = m_Transform.translation;
    bool isBeingPushed = false;
    float moveDistance = 0.0f;

    if (auto nick = App::GetInstance().GetNick()) {
        float nickSpeed = nick->GetSpeed();
        glm::vec2 nickPos = nick->GetPosition();
        float distance = glm::distance(position, nickPos);
        float collisionThreshold = (nick->GetCharacterWidth() + m_Width) / 2;

        if(m_SnowballState == SnowballState::Kicked || m_SnowballState == SnowballState::Killed) {
            int IsOnEdge = 0;
            m_RollingTimer += deltaTime;
            float moveDistance = (m_Direction == Direction::Right) ? m_RollingSpeed : -m_RollingSpeed;
            position = CalculatePosition(m_Width, m_Height, moveDistance * deltaTime);

            if (position.x >= (Map::MAP_WIDTH * Map::TILE_SIZE - 46.0f) / 2
                || position.x <= (-Map::MAP_WIDTH * Map::TILE_SIZE + 46.0f) / 2
                || GameWorld::CollisionToWall(position, m_Width, m_Height - 10, m_IsOnPlatform)) {
                m_Direction = (m_Direction == Direction::Right) ? Direction::Left : Direction::Right;
                ++IsOnEdge;
            }
            if(m_RollingTimer >= m_RollingDuration || (position.y <= -325.0f && IsOnEdge > 1)) {
                m_SnowballState = SnowballState::Killed;
            }

            if (m_SnowballState == SnowballState::Kicked) {
                CheckCollisionWithEnemies();
            }
        }else {
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
                m_SnowballState = SnowballState::Pushed;
                bool snowballOnPlatform = false;
                float dummyVelocity = 0.0f;
                glm::vec2 newPosition = GameWorld::map_collision_judgement(
                    m_Width, m_Height, position, dummyVelocity, -800.0f, moveDistance, snowballOnPlatform);

                float nickRight = nickPos.x + nick->GetCharacterWidth() / 2;
                float nickLeft = nickPos.x - nick->GetCharacterWidth() / 2;
                float snowballRight = newPosition.x + m_Width / 2;
                float snowballLeft = newPosition.x - m_Width / 2;

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
    }

    if (!isBeingPushed && m_SnowballState != SnowballState::Kicked && m_SnowballState != SnowballState::Killed) {
        m_SnowballState = SnowballState::Static;
        position = CalculatePosition(m_Width, m_Height);
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

void Snowball::OnKick(Direction direction) {
    m_SnowballState = SnowballState::Kicked;
    m_MeltTimer = 0.0f;
    SetMeltStage(0);
    std::shared_ptr<Util::Animation> animation = m_Animations["snowball_roll"];
    animation->Play();
    m_Direction = direction;
}

void Snowball::CheckCollisionWithEnemies() {
    auto& objects = GameWorld::GetObjects();
    glm::vec2 snowballPos = GetPosition();
    float snowballLeft = snowballPos.x - m_Width / 2;
    float snowballRight = snowballPos.x + m_Width / 2;
    float snowballTop = snowballPos.y + m_Height / 2;
    float snowballBottom = snowballPos.y - m_Height / 2;

    for (auto& obj : objects) {
        if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
            if (auto source = m_SourceEnemy.lock()) {
                if (source == enemy) continue; // 跳過來源敵人
            }

            // if (enemy->IsBoss()) {
            //     App::GetInstance().AddRemovingObject(shared_from_this());
            //     enemy->GetAttacked(); //
            // };

            glm::vec2 enemyPos = enemy->GetPosition();
            float enemyLeft = enemyPos.x - enemy->GetWidth() / 2;
            float enemyRight = enemyPos.x + enemy->GetWidth() / 2;
            float enemyTop = enemyPos.y + enemy->GetHeight() / 2;
            float enemyBottom = enemyPos.y - enemy->GetHeight() / 2;

            if (snowballRight > enemyLeft && snowballLeft < enemyRight &&
                snowballTop > enemyBottom && snowballBottom < enemyTop) {
                if (enemy->IsBoss()) { App::GetInstance().AddRemovingObject(shared_from_this()); }
                enemy->GetAttacked(); //
            }
        }
    }
}

glm::vec2 Snowball::CalculatePosition(float width, float height, float moveDistance) {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    float gravity = -800.0f;

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