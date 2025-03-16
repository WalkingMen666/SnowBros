#include "GameWorld.hpp"

#include "Util/Logger.hpp"

std::vector<std::shared_ptr<UpdatableDrawable>> GameWorld::m_Objects;

std::vector<std::shared_ptr<UpdatableDrawable>>& GameWorld::GetObjects() {
    return m_Objects;
}

void GameWorld::AddObject(std::shared_ptr<UpdatableDrawable> obj) {
    LOG_DEBUG("Adding object to GameWorld");
    m_Objects.push_back(obj);
}

void GameWorld::RemoveObject(std::shared_ptr<UpdatableDrawable> obj) {
    auto it = std::find(m_Objects.begin(), m_Objects.end(), obj);
    if (it != m_Objects.end()) {
        m_Objects.erase(it);
    }
}