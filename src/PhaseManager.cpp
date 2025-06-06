#include "PhaseResourceManger.hpp"
#include "Util/Input.hpp"

PhaseResourceManger::PhaseResourceManger() {
    m_Background = std::make_shared<BackgroundImage>();
    LoadPhase(-1); // 初始載入 phase-1
}

int PhaseResourceManger::GetPhase() const {
    return m_Phase;
}

Map& PhaseResourceManger::GetMap() {
    return m_Map;
}

const Map& PhaseResourceManger::GetMap() const {
    return m_Map;
}

void PhaseResourceManger::NextPhase() {
    m_Phase++;
    LoadPhase(m_Phase);
}

std::vector<std::shared_ptr<Util::GameObject>> PhaseResourceManger::GetChildren() const {
    return {m_Background};
}

void PhaseResourceManger::LoadPhase(int phase) {
    std::string filename = RESOURCE_DIR "/maps/phase" + std::to_string(phase) + ".map";
    try {
        m_Map.LoadFromFile(filename);
        m_Background->NextPhase(phase);
    } catch (const std::exception&) {
        m_Map = Map();
        std::vector<std::vector<int>> defaultMap(Map::MAP_HEIGHT, std::vector<int>(Map::MAP_WIDTH, 0));
        defaultMap[Map::MAP_HEIGHT - 1] = std::vector<int>(Map::MAP_WIDTH, 1); // 底部設為地面
        m_Map.LoadFromData(defaultMap);
        m_Background->NextPhase(phase);
    }
}