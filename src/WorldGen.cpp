#include "WorldGen.h"
#include "../CPLibrary/CPLibrary.h"
#include <cassert>

using namespace CPL;

std::unordered_set<glm::vec3, WorldGen::Vec3Hash> WorldGen::m_UniquePos;

std::vector<glm::vec3> WorldGen::GenTerrain(const glm::ivec3 &size) {
    bool isOdd = size.x % 2 == 1 && size.z % 2 == 1;
    assert(isOdd && "x and z coordinates must be odd to center the terrain!");

    std::vector<glm::vec3> blockPos;
    blockPos.reserve(static_cast<int32_t>(size.x * size.y * size.z));

    for (int x = -static_cast<int>(size.x / 2);
         x < static_cast<int>(size.x / 2); x++) {
        for (int y = 0; y < size.y; y++) {
            for (int z = -static_cast<int>(size.z / 2);
                 z < static_cast<int>(size.z / 2); z++) {
                blockPos.emplace_back(static_cast<float>(x) * 0.2f,
                                      static_cast<float>(y) * 0.2f,
                                      static_cast<float>(z) * 0.2f);
            }
        }
    }
    return blockPos;
}

std::vector<glm::ivec3> WorldGen::GenTrees(const glm::ivec3 &terrainSize,
                                           const int count) {
    std::vector<glm::ivec3> treePos;
    treePos.reserve(count);

    int minX = 0;
    int maxX = terrainSize.x - 1;
    int y = terrainSize.y;
    int minZ = 0;
    int maxZ = terrainSize.z - 1;

    for (int i = 0; i < count; i++) {
        glm::ivec3 pos(RandInt(minX, maxX), y, RandInt(minZ, maxZ));

        treePos.push_back(pos);
    }

    return treePos;
}

std::vector<std::vector<glm::vec3>>
WorldGen::GenFoliage(const glm::ivec3 &terrainSize, const int grassCount,
                     const int flowerCount) {
    return {m_GenGrass(terrainSize, grassCount),
            m_GenFlowers(terrainSize, flowerCount)};
}

std::vector<glm::vec3> WorldGen::m_GenGrass(const glm::ivec3 &terrainSize,
                                            const int count) {
    std::vector<glm::vec3> grassPos;
    grassPos.reserve(count);

    int minX = -static_cast<int>(terrainSize.x / 2);
    int maxX = static_cast<int>(terrainSize.x / 2) - 1;
    int y = terrainSize.y;
    int minZ = -static_cast<int>(terrainSize.z / 2);
    int maxZ = static_cast<int>(terrainSize.z / 2) - 1;

    while (grassPos.size() < static_cast<size_t>(count)) {
        glm::vec3 pos(static_cast<float>(RandInt(minX, maxX)) * 0.2f,
                      static_cast<float>(y) * 0.2f,
                      static_cast<float>(RandInt(minZ, maxZ)) * 0.2f);

        if (m_UniquePos.insert(pos).second) {
            grassPos.push_back(pos);
        }
    }

    return grassPos;
}

std::vector<glm::vec3> WorldGen::m_GenFlowers(const glm::ivec3 &terrainSize,
                                              const int count) {
    std::vector<glm::vec3> flowerPos;
    flowerPos.reserve(count);

    int minX = -static_cast<int>(terrainSize.x / 2);
    int maxX = static_cast<int>(terrainSize.x / 2) - 1;
    int y = terrainSize.y;
    int minZ = -static_cast<int>(terrainSize.z / 2);
    int maxZ = static_cast<int>(terrainSize.z / 2) - 1;

    while (flowerPos.size() < static_cast<size_t>(count)) {
        glm::vec3 pos(static_cast<float>(RandInt(minX, maxX)) * 0.2f,
                      static_cast<float>(y) * 0.2f,
                      static_cast<float>(RandInt(minZ, maxZ)) * 0.2f);

        if (m_UniquePos.insert(pos).second) {
            flowerPos.push_back(pos);
        }
    }

    return flowerPos;
}
