#include "PhaseResourceManger.hpp"
#include "Util/Logger.hpp"

PhaseResourceManger::PhaseResourceManger() {
    m_Background = std::make_shared<BackgroundImage>();
    LoadPhase(0); // 預設第0關
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
    if (m_Phase > 1) {
        m_Phase = 0; // 回到開始
    }
    LoadPhase(m_Phase);
    LOG_DEBUG("Switched to Phase: {}", m_Phase);
}

[[nodiscard]] std::vector<std::shared_ptr<Util::GameObject>> PhaseResourceManger::GetChildren() const {
    return {m_Background};
}

void PhaseResourceManger::LoadPhase(int phase) {
    std::string filename = RESOURCE_DIR "/maps/phase" + std::to_string(phase) + ".map";
    LOG_DEBUG("Attempting to load map file: {}", filename);
    try {
        m_Map.LoadFromFile(filename);
        LOG_DEBUG("Successfully loaded map file for phase {}", phase);
        
        // 打印地图的一些信息来验证加载
        for (int y = 0; y < Map::MAP_HEIGHT; ++y) {
            std::string row;
            for (int x = 0; x < Map::MAP_WIDTH; ++x) {
                row += std::to_string(m_Map.GetTile(x, y));
            }
            LOG_DEBUG("Map Row {}: {}", y, row);
        }
        
        m_Background->NextPhase(phase);
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load phase {}: {}", phase, e.what());
        // 使用內建預設地圖而不是拋出異常
        m_Map = Map(); // 重置地圖
        std::vector<std::vector<int>> defaultMap(Map::MAP_HEIGHT, std::vector<int>(Map::MAP_WIDTH, 0));
        defaultMap[Map::MAP_HEIGHT - 1] = std::vector<int>(Map::MAP_WIDTH, 1); // 底部設為地面
        m_Map.LoadFromData(defaultMap); // 使用 LoadFromData 而不是直接賦值
        LOG_DEBUG("Loaded default in-memory map for phase {}", phase);
        m_Background->NextPhase(phase);
    }
}
