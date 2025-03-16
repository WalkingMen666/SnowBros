#include "GameWorld.hpp"
#include "Util/Logger.hpp"

std::vector<std::shared_ptr<UpdatableDrawable>> GameWorld::m_Objects;

std::vector<std::shared_ptr<UpdatableDrawable>>& GameWorld::GetObjects() {
    return m_Objects;
}

void GameWorld::AddObject(std::shared_ptr<UpdatableDrawable> obj) {
    LOG_INFO("Adding object to GameWorld");
    m_Objects.push_back(obj);
}

void GameWorld::RemoveObject(std::shared_ptr<UpdatableDrawable> obj) {
    auto it = std::find(m_Objects.begin(), m_Objects.end(), obj);
    if (it != m_Objects.end()) {
        m_Objects.erase(it);
    }
}

glm::vec2 GameWorld::map_collision_judgement(float characterWidth, float characterHeight, glm::vec2 position, float &m_JumpVelocity, float m_Gravity, float moveDistance, bool &isOnPlatform) {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    const float m_GroundLevel = -285.0f;
    glm::vec2 newPosition = position;

    auto prm = App::GetPRM();
    if (!prm) {
        LOG_ERROR("PRM is null!");
        return newPosition;
    }
    const Map& map = prm->GetMap();

    float characterBottom = position.y - characterHeight / 2;
    float characterTop = position.y + characterHeight / 2;
    float characterLeft = position.x - characterWidth / 2;
    float characterRight = position.x + characterWidth / 2;

    m_JumpVelocity += m_Gravity * deltaTime;
    float nextY = newPosition.y + m_JumpVelocity * deltaTime;
    float nextBottom = nextY - characterHeight / 2;
    float nextTop = nextY + characterHeight / 2;

    if(moveDistance != 0) {
        float nextX = position.x + moveDistance;
        float nextLeft = nextX - characterWidth / 2;
        float nextRight = nextX + characterWidth / 2;

        // 檢查水平和垂直範圍（縮小垂直範圍至 ±5 單位）
        int startTileX = std::max(0, std::min(static_cast<int>((std::min(characterLeft, nextLeft) + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        int endTileX = std::max(0, std::min(static_cast<int>((std::max(characterRight, nextRight) + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        float minY = std::min(characterBottom, nextBottom) - 4.775f; // 下擴展 4.775 單位
        float maxY = std::max(characterTop, nextTop) + 5.0f;       // 上擴展 5 單位
        int tileYBottom = std::max(0, std::min(static_cast<int>((360.0f - minY) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
        int tileYTop = std::max(0, std::min(static_cast<int>((360.0f - maxY) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));

        bool willCollide = false;
        for (int tileX = startTileX; tileX <= endTileX && !willCollide; ++tileX) {
            for (int y = tileYTop; y <= tileYBottom && !willCollide; ++y) {
                if (map.GetTile(tileX, y) == 2) {
                    float tileLeft = tileX * Map::TILE_SIZE - 410.0f;
                    float tileRight = tileLeft + Map::TILE_SIZE;

                    // 向右移動
                    if (moveDistance > 0) {
                        if (characterRight <= tileLeft && nextRight > tileLeft) {
                            willCollide = true;
                            newPosition.x = position.x; // 停在原地
                            break;
                        }
                    }
                    // 向左移動
                    else if (moveDistance < 0) {
                        if (characterLeft >= tileRight && nextLeft < tileRight) {
                            willCollide = true;
                            newPosition.x = position.x; // 停在原地
                            break;
                        }
                    }
                }
            }
        }
        // 若未碰撞，應用移動並檢查邊界
        if (!willCollide) {
            newPosition.x = nextX;
            newPosition.x = std::clamp(nextX, -410.0f + characterWidth / 2, 410.0f - characterWidth / 2);
        }
    }

    // 平台碰撞檢測（僅在下落時）
    float platformY = m_GroundLevel; // 地面高度
    if (m_JumpVelocity <= 0) { // 下落或靜止時檢查
        int leftTileX = std::max(0, std::min(static_cast<int>((newPosition.x - characterWidth / 2 + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        int rightTileX = std::max(0, std::min(static_cast<int>((newPosition.x + characterWidth / 2 + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        int tileYStart = std::max(0, std::min(static_cast<int>((360.0f - characterBottom) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
        int tileYEnd = std::max(0, std::min(static_cast<int>((360.0f - nextBottom) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));

        for (int tileX = leftTileX; tileX <= rightTileX; ++tileX) {
            for (int tileY = std::min(tileYStart, tileYEnd); tileY <= std::max(tileYStart, tileYEnd); ++tileY) {
                if (tileY >= 0 && tileY < 144 && map.GetTile(tileX, tileY) == 1) {
                    float platformTop = 360.0f - tileY * Map::TILE_SIZE;
                    if (characterBottom >= platformTop && nextBottom <= platformTop) {
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

    // 更新位置
    if (isOnPlatform) {
        newPosition.y = platformY;
    } else {
        newPosition.y = nextY;
        if (newPosition.y <= m_GroundLevel) {
            newPosition.y = m_GroundLevel;
            m_JumpVelocity = 0.0f;
            isOnPlatform = true;
        }
    }

    return newPosition;
}