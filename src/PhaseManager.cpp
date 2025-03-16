#include "PhaseResourceManger.hpp"
#include "Util/Logger.hpp"
#include "Util/Input.hpp"

PhaseResourceManger::PhaseResourceManger() {
    m_Background = std::make_shared<BackgroundImage>();
    LoadPhase(-1); // 初始載入 phase_1.map，對應 Phase_1
    // m_Phase = -1; // 同步 m_Phase 為 1
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
        m_Background->NextPhase(phase);
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load phase {}: {}", phase, e.what());
        m_Map = Map();
        std::vector<std::vector<int>> defaultMap(Map::MAP_HEIGHT, std::vector<int>(Map::MAP_WIDTH, 0));
        defaultMap[Map::MAP_HEIGHT - 1] = std::vector<int>(Map::MAP_WIDTH, 1); // 底部設為地面
        m_Map.LoadFromData(defaultMap);
        LOG_DEBUG("Loaded default in-memory map for phase {}", phase);
        m_Background->NextPhase(phase);
    }
}
glm::vec2 PhaseResourceManger::CheckAndSwitchPhase(const glm::vec2& nickPosition) {
    glm::vec2 newPosition = nickPosition; // 預設返回當前位置

    if (!m_HasSwitchedPhase) { // 僅在未切換時檢查
        bool goToNextLevel = false;

        // 作弊功能：按 N 鍵直接進入下一關（單次觸發）
        static bool wasNPressed = false;
        bool isNPressed = Util::Input::IsKeyPressed(Util::Keycode::N);
        if (isNPressed && !wasNPressed) {
            goToNextLevel = true;
        }
        wasNPressed = isNPressed;

        if (goToNextLevel) {
            NextPhase();                    // 切換到下一關
            newPosition = {0.0f, -285.0f}; // 重置位置
            m_HasSwitchedPhase = true;
            LOG_INFO("Entering Phase: {}", m_Phase);
        }
    } else if (nickPosition.x < 405.0f) {
        // 若離開觸發區，重置標誌
        m_HasSwitchedPhase = false;
    }

    return newPosition; // 返回新位置（若未切換則為原位置）
}