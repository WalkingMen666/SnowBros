#ifndef GAME_WORLD_HPP
#define GAME_WORLD_HPP

#include <vector>
#include <memory>
#include "Util/Time.hpp"
#include "UpdatableDrawable.hpp"
#include "Map.hpp"
#include "PhaseResourceManger.hpp"
#include "App.hpp"

class GameWorld {
public:
    static std::vector<std::shared_ptr<UpdatableDrawable>>& GetObjects();
    static void AddObject(std::shared_ptr<UpdatableDrawable> obj);
    static void RemoveObject(std::shared_ptr<UpdatableDrawable> obj);
    static glm::vec2 map_collision_judgement(
        float characterWidth,
        float characterHeight,
        glm::vec2 position,
        float &m_JumpVelocity,
        float m_Gravity,
        float moveDistance,
        bool &isOnPlatform);
    static bool CollisionToWall(const glm::vec2& position, float width, float height, bool isOnPlatform);

private:
    static std::vector<std::shared_ptr<UpdatableDrawable>> m_Objects;
};

#endif // GAME_WORLD_HPP