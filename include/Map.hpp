#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>

class Map {
public:
    static const int TILE_SIZE = 5;
    static const int MAP_WIDTH = 164;
    static const int MAP_HEIGHT = 152;

    Map() = default;

    void LoadFromFile(const std::string& filename);   // 從檔案載入地圖
    void LoadFromData(const std::vector<std::vector<int>>& data);  // 從數據載入地圖

    int GetTile(int x, int y) const;  // 取得指定座標的瓦片資訊

private:
    std::vector<std::vector<int>> m_TileMap;  // 地圖儲存結構
};

#endif // MAP_HPP
