#include "GameWorld.hpp"
#include "Enemy.hpp"

std::vector<std::shared_ptr<UpdatableDrawable>> GameWorld::m_Objects;

std::vector<std::shared_ptr<UpdatableDrawable>>& GameWorld::GetObjects() {
    return m_Objects;
}

void GameWorld::AddObject(std::shared_ptr<UpdatableDrawable> obj) {
    m_Objects.push_back(std::move(obj));
}

void GameWorld::RemoveObject(std::shared_ptr<UpdatableDrawable> obj) {
    auto it = std::find(m_Objects.begin(), m_Objects.end(), obj);
    if (it != m_Objects.end()) {
        m_Objects.erase(it);
    }
}

glm::vec2 GameWorld::map_collision_judgement(float characterWidth, float characterHeight, glm::vec2 position,
                                             float& m_JumpVelocity, float m_Gravity, float moveDistance, bool& isOnPlatform) {
    static const float GROUND_LEVEL = -325.0f;
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    auto prm = App::GetPRM();
    if (!prm) {
        LOG_ERROR("GameWorld::map_collision_judgement: PhaseResourceManager is not initialized!");
        return position;
    }
    const Map& map = prm->GetMap();

    struct Bounds {
        float bottom, top, left, right;
    };
    Bounds current{position.y - characterHeight / 2, position.y + characterHeight / 2,
                   position.x - characterWidth / 2, position.x + characterWidth / 2};

    m_JumpVelocity += m_Gravity * deltaTime;
    float nextY = position.y + m_JumpVelocity * deltaTime;
    Bounds next{nextY - characterHeight / 2, nextY + characterHeight / 2, current.left, current.right};

    if (moveDistance != 0) {
        float nextX = position.x + moveDistance;
        next.left = nextX - characterWidth / 2;
        next.right = nextX + characterWidth / 2;

        int startTileX = std::max(0, std::min(static_cast<int>((std::min(current.left, next.left) + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        int endTileX = std::max(0, std::min(static_cast<int>((std::max(current.right, next.right) + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        int tileYTop = std::max(0, std::min(static_cast<int>((360.0f - std::max(current.top, next.top) + 5.0f) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
        int tileYBottom = std::max(0, std::min(static_cast<int>((360.0f - std::min(current.bottom, next.bottom) - 4.775f) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));

        for (int tileX = startTileX; tileX <= endTileX; ++tileX) {
            for (int y = tileYTop; y <= tileYBottom; ++y) {
                if (map.GetTile(tileX, y) == 2) {
                    float tileLeft = tileX * Map::TILE_SIZE - 410.0f;
                    float tileRight = tileLeft + Map::TILE_SIZE;

                    if (moveDistance > 0 && current.right <= tileLeft && next.right > tileLeft) {
                        nextX = tileLeft - characterWidth / 2;
                        break;
                    } else if (moveDistance < 0 && current.left >= tileRight && next.left < tileRight) {
                        nextX = tileRight + characterWidth / 2;
                        break;
                    }
                }
            }
        }
        position.x = std::clamp(nextX, -410.0f + characterWidth / 2, 410.0f - characterWidth / 2);
    }

    isOnPlatform = false;
    float platformY = GROUND_LEVEL;
    int leftTileX = std::max(0, std::min(static_cast<int>((current.left + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
    int rightTileX = std::max(0, std::min(static_cast<int>((current.right + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
    int tileYCurrent = std::max(0, std::min(static_cast<int>((360.0f - current.bottom) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
    int tileYNext = std::max(0, std::min(static_cast<int>((360.0f - next.bottom) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));

    bool foundPlatform = false;
    for (int tileX = leftTileX; tileX <= rightTileX && !foundPlatform; ++tileX) {
        for (int tileY = std::min(tileYCurrent, tileYNext); tileY <= std::max(tileYCurrent, tileYNext); ++tileY) {
            if (tileY >= 0 && tileY < Map::MAP_HEIGHT && map.GetTile(tileX, tileY) == 1) {
                float platformTop = 360.0f - tileY * Map::TILE_SIZE;
                if (current.bottom >= platformTop && next.bottom <= platformTop) {
                    foundPlatform = true;
                    isOnPlatform = true;
                    platformY = platformTop + characterHeight / 2;
                    m_JumpVelocity = 0.0f;
                    break;
                }
            }
        }
    }

    if (!foundPlatform && m_JumpVelocity <= 0) {
        for (const auto& obj : m_Objects) {
            if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                if (enemy->GetState() == EnemyState::Snowball) {
                    glm::vec2 enemyPos = enemy->GetPosition();
                    float enemyTop = enemyPos.y + enemy->GetCharacterHeight() / 2;
                    float enemyLeft = enemyPos.x - enemy->GetCharacterWidth() / 2;
                    float enemyRight = enemyPos.x + enemy->GetCharacterWidth() / 2;

                    if (current.bottom >= enemyTop && next.bottom <= enemyTop &&
                        current.right > enemyLeft && current.left < enemyRight) {
                        foundPlatform = true;
                        isOnPlatform = true;
                        platformY = enemyTop + characterHeight / 2;
                        m_JumpVelocity = 0.0f;
                        break;
                    }
                }
            }
        }
    }

    if (foundPlatform) {
        position.y = platformY;
    } else {
        position.y = nextY;
        if (position.y <= GROUND_LEVEL) {
            position.y = GROUND_LEVEL;
            m_JumpVelocity = 0.0f;
            isOnPlatform = true;
        }
    }
    return position;
}

bool GameWorld::CollisionToWall(const glm::vec2& position, float width, float height, bool isOnPlatform) {
    auto prm = App::GetPRM();
    if (!prm) {
        LOG_ERROR("GameWorld::CollisionToWall: PhaseResourceManager is not initialized");
        return false;
    }
    const Map& map = prm->GetMap();

    int leftTileX = std::max(0, std::min(static_cast<int>((position.x - width / 2 + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
    int rightTileX = std::max(0, std::min(static_cast<int>((position.x + width / 2 + 410.0f) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
    int topTileY = std::max(0, std::min(static_cast<int>((360.0f - position.y - height / 2) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
    int bottomTileY = std::max(0, std::min(static_cast<int>((360.0f - (position.y - height / 2) + 5.0f) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
    int graceVal = 1;

    for(int tileY = topTileY; tileY <= bottomTileY; ++tileY) {
        for(int tileX = leftTileX - graceVal; tileX <= leftTileX + graceVal; ++tileX) {
            if(isOnPlatform && map.GetTile(tileX, tileY) == 2) return true;
        }
        for(int tileX = rightTileX - graceVal; tileX <= rightTileX + graceVal; ++tileX) {
            if(isOnPlatform && map.GetTile(tileX, tileY) == 2) return true;
        }
    }
    return false;
}
