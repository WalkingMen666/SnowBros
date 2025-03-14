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

private:
    void LoadPhase(int phase);  // 讀取關卡資源

    std::shared_ptr<BackgroundImage> m_Background;
    Map m_Map;
    int m_Phase = -1;
};

#endif // PHASE_MANGER_HPP
