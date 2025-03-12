#include "Map.hpp"
#include <fstream>
#include <stdexcept>
#include "Util/Logger.hpp"

void Map::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open map file: {}", filename);
        throw std::runtime_error("Failed to open map file: " + filename);
    }

    LOG_DEBUG("Started loading map from file: {}", filename);
    m_TileMap.clear();
    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < MAP_HEIGHT) {
        std::vector<int> rowData;
        for (char c : line) {
            if (rowData.size() >= MAP_WIDTH) break;
            if (c >= '0' && c <= '2') {
                rowData.push_back(c - '0');
            }
        }
        while (rowData.size() < MAP_WIDTH) {
            rowData.push_back(0);
        }
        m_TileMap.push_back(rowData);
        LOG_DEBUG("Loaded row {}: {} tiles", row, rowData.size());
        row++;
    }
    while (m_TileMap.size() < MAP_HEIGHT) {
        m_TileMap.push_back(std::vector<int>(MAP_WIDTH, 0));
    }
    file.close();
    LOG_DEBUG("Finished loading map from file: {} with {} rows", filename, m_TileMap.size());
}

void Map::LoadFromData(const std::vector<std::vector<int>>& data) {
    LOG_DEBUG("Loading map from data with {} rows", data.size());
    m_TileMap = data;
    // 確保地圖尺寸正確
    while (m_TileMap.size() < MAP_HEIGHT) {
        m_TileMap.push_back(std::vector<int>(MAP_WIDTH, 0));
    }
    for (auto& row : m_TileMap) {
        while (row.size() < MAP_WIDTH) {
            row.push_back(0);
        }
    }
    LOG_DEBUG("Finished loading map from data, final size: {}x{}", m_TileMap.size(), m_TileMap[0].size());
}

int Map::GetTile(int x, int y) const {
    if (m_TileMap.empty() || x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return 0;
    }
    return m_TileMap[y][x];
}
