#ifndef GAME_WORLD_HPP
#define GAME_WORLD_HPP

#include <vector>
#include <memory>
#include "UpdatableDrawable.hpp"

class GameWorld {
public:
    static std::vector<std::shared_ptr<UpdatableDrawable>>& GetObjects();
    static void AddObject(std::shared_ptr<UpdatableDrawable> obj);
    static void RemoveObject(std::shared_ptr<UpdatableDrawable> obj);
private:
    static std::vector<std::shared_ptr<UpdatableDrawable>> m_Objects;
};

#endif // GAME_WORLD_HPP