#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <unordered_set>

class WorldGen {
  public:
    static std::vector<glm::vec3> GenTerrain(const glm::ivec3 &size);
    static std::vector<glm::vec3> GenTrees(const glm::ivec3 &terrainSize,
                                           int count);
    static std::vector<std::vector<glm::vec3>>
    GenFoliage(const glm::ivec3 &terrainSize, int grassCount, int flowerCount);
    
  private:
    struct Vec3Hash {
        std::size_t operator()(const glm::vec3 &v) const {
            return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1) ^
                   (std::hash<float>()(v.z) << 2);
        }
    };

    static std::unordered_set<glm::vec3, Vec3Hash> m_UniquePos;

    static std::vector<glm::vec3> m_GenGrass(const glm::ivec3 &terrainSize,
                                             int count);
    static std::vector<glm::vec3> m_GenFlowers(const glm::ivec3 &terrainSize,
                                               int count);
};
