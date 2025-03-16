#include "RedDemon.hpp"
#include "Nick.hpp"
#include "Snowball.hpp"
#include "Map.hpp"
#include "App.hpp"
#include "Util/Logger.hpp"

RedDemon::RedDemon(const glm::vec2& pos) : Enemy(RESOURCE_DIR "/Image/Character/Enemies/red_stand_right.png", pos) {
    LoadAnimations();
    SetAnimation("stand_right");
    m_Drawable = m_Animations["stand_right"];
}

void RedDemon::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    if (m_State == EnemyState::Normal) {
        glm::vec2 position = GetPosition();
        // 水平移動
        float moveSpeed = m_speed * deltaTime; // 調整速度
        float moveDistance = 0.0f;
        bool isOnPlatform = false;
        moveDistance = (m_Direction == Direction::Right) ? moveSpeed : -moveSpeed;
        SetAnimation((m_Direction == Direction::Right) ? "walk_right" : "walk_left");

        // 重力與跳躍
        m_JumpVelocity += m_Gravity * deltaTime;
        glm::vec2 newPosition = GameWorld::map_collision_judgement(characterWidth, characterWidth, position, m_JumpVelocity, m_Gravity, moveDistance, isOnPlatform);
        newPosition.y += m_JumpVelocity * deltaTime;
        SetPosition(newPosition);
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