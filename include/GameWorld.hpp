#ifndef GAME_WORLD_HPP
#define GAME_WORLD_HPP

#include <vector>
#include <memory>
#include "Util/Time.hpp"
#include "UpdatableDrawable.hpp"
#include "Map.hpp"
#include "PhaseResourceManger.hpp"
#include "App.hpp"
#include "Nick.hpp"
#include "Enemy.hpp"
#include "ScoreItem.hpp"

class GameWorld : public UpdatableDrawable {
public:
    GameWorld();
    void Update() override;
    float GetWidth() const override;
    float GetHeight() const override;
    std::shared_ptr<Core::Drawable> GetDrawable() const override;

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
        bool &isOnPlatform,
        bool stepOnSnowBall = true);
    static bool CollisionToWall(const glm::vec2& position, float width, float height, bool isOnPlatform);

    // 獲取所有敵人
    const std::vector<std::shared_ptr<Enemy>>& GetEnemies() const { return m_Enemies; }

    // 獲取所有分數物品
    const std::vector<std::shared_ptr<ScoreItem>>& GetScoreItems() const { return m_ScoreItems; }

    // 添加分數物品
    void AddScoreItem(std::shared_ptr<ScoreItem> item) { m_ScoreItems.push_back(item); }

private:
    static std::vector<std::shared_ptr<UpdatableDrawable>> m_Objects;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::vector<std::shared_ptr<ScoreItem>> m_ScoreItems;
    std::shared_ptr<Nick> m_Nick;

    // void UpdateScoreItems();
    // void CheckScoreItemCollisions();
};

#endif // GAME_WORLD_HPP