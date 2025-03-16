#include "RedDemon.hpp"
#include "Nick.hpp"
#include "Snowball.hpp"
#include "Map.hpp"
#include "App.hpp"
#include "Util/Logger.hpp"

RedDemon::RedDemon(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Enemies/red_stand_right.png", pos) {
    LoadAnimations();
    SetAnimation("stand_right");
    Util::GameObject::m_Drawable = m_Animations["stand_right"];
}

void RedDemon::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    if (m_State == EnemyState::Normal) {
        glm::vec2 position = GetPosition();
        glm::vec2 newPosition = position;

        const float characterWidth = 46.0f, characterHeight = 46.0f;
        float characterBottom = position.y - characterHeight / 2;
        float characterTop = position.y + characterHeight / 2;
        float characterLeft = position.x - characterWidth / 2;
        float characterRight = position.x + characterWidth / 2;
        const float VERTICAL_CHECK_RANGE = 5.0f;
        // 水平移動
        float moveSpeed = m_speed * deltaTime; // 調整速度
        newPosition.x += (m_Direction == Direction::Right) ? moveSpeed : -moveSpeed;
        SetAnimation((m_Direction == Direction::Right) ? "walk_right" : "walk_left");

        // 重力與跳躍
        m_JumpVelocity += m_Gravity * deltaTime;
        newPosition.y += m_JumpVelocity * deltaTime;

        // 地圖碰撞（參考 Nick）
        auto prm = App::GetPRM();
        if (prm) {
            const Map& map = prm->GetMap();
            float left = newPosition.x - characterWidth / 2;
            float right = newPosition.x + characterWidth / 2;
            float bottom = newPosition.y - characterHeight / 2;
            float top = newPosition.y + characterHeight / 2;

            int leftTileX = std::max(0, std::min(static_cast<int>((std::min(characterLeft,left)+ 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
            int rightTileX = std::max(0, std::min(static_cast<int>((std::min(characterRight,right)+ 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
            float minY = std::min(characterBottom, bottom) - VERTICAL_CHECK_RANGE;
            float maxY = std::max(characterTop, top) + VERTICAL_CHECK_RANGE;
            int tileYBottom = std::max(0, std::min(static_cast<int>((360.0f - minY) / Map::Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
            int tileYTop = std::max(0, std::min(static_cast<int>((360.0f - maxY) / Map::Map::TILE_SIZE), Map::MAP_HEIGHT - 1));

            bool willCollide = false;
            for (int tileX = leftTileX; tileX <= rightTileX && !willCollide; ++tileX) {
                for (int y = tileYTop; y <= tileYBottom && !willCollide; ++y) {
                    if (map.GetTile(tileX, y) == 2) {
                        float tileLeft = tileX * Map::Map::TILE_SIZE - 410.0f;
                        float tileRight = tileLeft + Map::Map::TILE_SIZE;

                        if (m_Direction == Direction::Right  && characterRight <= tileLeft && right > tileLeft) {
                            willCollide = true;
                            newPosition.x = tileLeft - characterWidth / 2;
                        } else if (m_Direction == Direction::Left && characterLeft >= tileRight && left < tileRight) {
                            willCollide = true;
                            newPosition.x = tileRight + characterWidth / 2;
                        }
                    }
                }
            }

            if (!willCollide) {
                newPosition.x = std::clamp(newPosition.x, -410.0f, 410.0f);
            }

            bool isOnPlatform = false;
            float platformY = m_GroundLevel; // 地面高度
            if (m_JumpVelocity <= 0) {
                int leftTileX = std::max(0, std::min(static_cast<int>((newPosition.x - characterWidth / 2 + 410.0f) / Map::TILE_SIZE), 164 - 1));
                int rightTileX = std::max(0, std::min(static_cast<int>((newPosition.x + characterWidth / 2 + 410.0f) / Map::TILE_SIZE), 164 - 1));
                int tileYStart = std::max(0, std::min(static_cast<int>((360.0f - characterBottom) / Map::TILE_SIZE), 144 - 1));
                int tileYEnd = std::max(0, std::min(static_cast<int>((360.0f - bottom) / Map::TILE_SIZE), 144 - 1));

                for (int tileX = leftTileX; tileX <= rightTileX; ++tileX) {
                    for (int tileY = std::min(tileYStart, tileYEnd); tileY <= std::max(tileYStart, tileYEnd); ++tileY) {
                        if (map.GetTile(tileX, tileY) == 1) {
                            float platformTop = 360.0f - tileY * Map::TILE_SIZE;
                            if (characterBottom >= platformTop && bottom <= platformTop) {
                                isOnPlatform = true;
                                platformY = platformTop + characterHeight / 2;
                                m_JumpVelocity = 0.0f;
                                break;
                            }
                        }
                    }
                    if (isOnPlatform) break;
                }
            }

            if (isOnPlatform) {
                newPosition.y = platformY;
            } else {
                newPosition.y = std::max(newPosition.y, -285.0f);
                if (newPosition.y == -285.0f) m_JumpVelocity = 0.0f;
            }
            SetPosition(newPosition);
        }

        m_JumpTimer += deltaTime;
        if (m_JumpTimer >= JUMP_INTERVAL) {
            m_JumpTimer = 0.0f;
            if (rand() % 2 == 0) JumpToPlatform();
        }
    }
}

void RedDemon::JumpToPlatform() {
    auto prm = App::GetPRM();
    if (!prm) return;
    const Map& map = prm->GetMap();
    glm::vec2 currentPos = GetPosition();

    // 尋找最近平台
    float nearestDist = std::numeric_limits<float>::max();
    glm::vec2 targetPos = currentPos;
    for (int y = 0; y < Map::MAP_HEIGHT; ++y) {
        for (int x = 0; x < Map::MAP_WIDTH; ++x) {
            if (map.GetTile(x, y) == 1) {
                float platformX = x * Map::TILE_SIZE - 410.0f;
                float platformY = 360.0f - y * Map::TILE_SIZE;
                float dist = glm::distance(currentPos, glm::vec2(platformX, platformY));
                if (dist < nearestDist && std::abs(platformY - currentPos.y) <= 100.0f) { // 限制高度差
                    nearestDist = dist;
                    targetPos = glm::vec2(platformX, platformY);
                }
            }
        }
    }

    if (targetPos != currentPos) {
        m_JumpVelocity = m_JumpInitialVelocity; // 平滑跳躍
        SetAnimation((m_Direction == Direction::Right) ? "jump_right" : "jump_left");
    }
}

void RedDemon::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (auto nick = std::dynamic_pointer_cast<Nick>(other)) {
        nick->Die();
    } else if (auto snowball = std::dynamic_pointer_cast<Snowball>(other)) {
        if (snowball->IsMoving()) {
            m_State = EnemyState::Dead;
            SetAnimation("die");
            // 在App::Update中檢查動畫結束後移除
        }
    }
}

void RedDemon::LoadAnimations() {
    m_Animations["stand_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_stand_left.png"}, false, 500, true, 0);
    m_Animations["stand_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_stand_right.png"}, false, 500, true, 0);
    m_Animations["walk_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_walk_left_1.png", BASE_PATH + "red_walk_left_2.png"}, false, 200, true, 0);
    m_Animations["walk_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_walk_right_1.png", BASE_PATH + "red_walk_right_2.png"}, false, 200, true, 0);
    m_Animations["jump_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_jump_left.png"}, false, 300, false, 0);
    m_Animations["jump_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_jump_right.png"}, false, 300, false, 0);
    m_Animations["down_left"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_down_left.png"}, false, 300, false, 0);
    m_Animations["down_right"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_down_right.png"}, false, 300, false, 0);
    m_Animations["die"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "red_die_1.png", BASE_PATH + "red_die_2.png", BASE_PATH + "red_die_3.png",
                                 BASE_PATH + "red_die_4.png", BASE_PATH + "red_die_5.png"}, false, 500, false, 0);
    m_Animations["snowball_roll"] = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_roll_1.png", BASE_PATH + "snowball_roll_2.png",
                                 BASE_PATH + "snowball_roll_3.png", BASE_PATH + "snowball_roll_4.png"}, false, 200, true, 0);
    m_Drawable = m_Animations["stand_right"];
}