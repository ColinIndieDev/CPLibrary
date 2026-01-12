#pragma once

#include "ChunkManager.h"
#include "FastNoiseLite.h"
#include <glm/glm.hpp>

class WorldGen {
  public:
    uint32_t seed;
    ChunkManager manager;
    ChunkManager transparentManager;
    int viewDist;

    int minMapHeight;
    int maxMapHeight;
    int baseMapHeight;

    explicit WorldGen(const uint32_t seed, const int viewDist,
                      const int minMapHeight, const int maxMapHeight,
                      const int baseMapHeight)
        : seed(seed), manager(false), transparentManager(true),
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

  private:
    struct TerrainNoise {
        FastNoiseLite noise;
        FastNoiseLite mountain;
        FastNoiseLite peak;
    };
    struct CaveNoise {
        FastNoiseLite noise;
        FastNoiseLite region;
        FastNoiseLite entrance;
    };
    struct TreeNoise {
        FastNoiseLite noise;
    };

    TerrainNoise terrainNoise;
    CaveNoise caveNoise;
    TreeNoise treeNoise;

    void m_InitNoises();
};
