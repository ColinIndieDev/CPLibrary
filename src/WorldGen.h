#pragma once

#include "ChunkManager.h"
#include "FastNoiseLite.h"
#include <glm/glm.hpp>

class WorldGen {
  public:
    uint32_t seed;
    ChunkManager manager;
    int viewDist;

    int minMapHeight;
    int maxMapHeight;
    int baseMapHeight;

    explicit WorldGen(const uint32_t seed, const int viewDist,
                      const int minMapHeight, const int maxMapHeight,
                      const int baseMapHeight)
        : seed(seed),
          viewDist(viewDist), minMapHeight(minMapHeight),
          maxMapHeight(maxMapHeight), baseMapHeight(baseMapHeight) {}

    void Init();
    void GenMap();
    void UpdateMap();

    int GetTerrainHeight(int worldX, int worldZ) const;
    void GenTrees(int x, int z, int worldX, int worldZ, int height,
                  Chunk &chunk) const;
    void GenCaves(int x, int z, const glm::ivec3 &world, int height,
                  Chunk &chunk) const;
    std::pair<bool, float> GenHoles(int worldX, int worldZ) const;
    std::pair<bool, float> GenRivers(int worldX, int worldZ) const;
    std::pair<bool, float> GenOceans(int worldX, int worldZ) const;

  private:
    struct TerrainNoise {
        FastNoiseLite noise;
        FastNoiseLite mountain;
        FastNoiseLite peak;

        FastNoiseLite holes;
    };
    struct CaveNoise {
        FastNoiseLite noise;
        FastNoiseLite region;
        FastNoiseLite entrance;
    };
    struct TreeNoise {
        FastNoiseLite noise;
    };
    struct WaterNoise {
        FastNoiseLite river;
        FastNoiseLite ocean;
    };

    TerrainNoise terrainNoise;
    CaveNoise caveNoise;
    TreeNoise treeNoise;
    WaterNoise waterNoise;

    void m_InitNoises();
};
