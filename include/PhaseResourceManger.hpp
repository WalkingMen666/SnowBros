#ifndef PHASE_MANGER_HPP
#define PHASE_MANGER_HPP

#include "Util/GameObject.hpp"
#include "BackgroundImage.hpp"
#include "Map.hpp"

class PhaseResourceManger {
public:
    PhaseResourceManger(); // 構造函數

    int GetPhase() const;                // 關卡進度
    Map& GetMap();                       // 地圖資訊
    const Map& GetMap() const;           //

    void NextPhase();                    // 下一關
    [[nodiscard]] std::vector<std::shared_ptr<Util::GameObject>> GetChildren() const; // 取得子物件
    // 新增：檢查並切換關卡，返回新位置（若無切換則返回當前位置）
    glm::vec2 CheckAndSwitchPhase(const glm::vec2& nickPosition);

private:
    void LoadPhase(int phase);  // 讀取關卡資源

    std::shared_ptr<BackgroundImage> m_Background;
    Map m_Map;
    int m_Phase = -1;
    const float VERTICAL_CHECK_RANGE = 5.0f;
    bool m_HasSwitchedPhase = false; // 防止連續跳關
};

#endif // PHASE_MANGER_HPP
