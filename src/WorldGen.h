#pragma once

#include "ChunkManager.h"
#include "FastNoiseLite.h"
#include <glm/glm.hpp>

class WorldGen {
  public:
    uint32_t seed;
    ChunkManager manager;
    glm::ivec2 mapSize;

    int minMapHeight;
    int maxMapHeight;
    int baseMapHeight;

    explicit WorldGen(const uint32_t seed, const glm::ivec2 &mapSize,
                      const int minMapHeight, const int maxMapHeight,
                      const int baseMapHeight)
        : seed(seed), mapSize(mapSize), minMapHeight(minMapHeight),
          maxMapHeight(maxMapHeight), baseMapHeight(baseMapHeight) {}

    void Init();
    void GenMap();

    int GetTerrainHeight(int worldX, int worldZ);

  private:
    FastNoiseLite terrainNoise;
    FastNoiseLite treeNoise;
    FastNoiseLite mountainMask;
    FastNoiseLite peakNoise;
    FastNoiseLite caveNoise;
    FastNoiseLite caveRegionNoise;
    FastNoiseLite caveEntranceNoise;

    void m_InitNoises();
    void m_GenTrees(int x, int z, int worldX, int worldZ, int height,
                    Chunk &chunk);
    void m_GenCaves(int x, int z, const glm::ivec3 &world, int height, Chunk& chunk);
    void m_CreateChunks();
    void m_GenChunks();
    void m_CreateChunkMeshes();
};
