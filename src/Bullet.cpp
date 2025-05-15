#include "Bullet.hpp"
#include "Enemy.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"

const std::string Bullet::BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";

Bullet::Bullet(const glm::vec2& pos) : m_LifeTime(0.0f) {
    m_Transform.translation = pos;
    SetVisible(true);
    SetZIndex(5);
}

// 移除 Update 和 OnCollision 函數的實作，因為它們是純虛擬函數