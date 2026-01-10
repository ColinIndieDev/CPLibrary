#include "WorldGen.h"
#include "../CPLibrary/CPLibrary.h"
#include "Block.h"

void WorldGen::Init() { m_InitNoises(); }

uint32_t DeriveSeed(const uint32_t base, const uint32_t salt) {
    uint32_t h = base;
    h ^= salt + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
}

void WorldGen::m_InitNoises() {
    peakNoise.SetSeed(static_cast<int>(DeriveSeed(seed, 76767676)));
    peakNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    peakNoise.SetFrequency(0.0008f);

    mountainMask.SetSeed(static_cast<int>(DeriveSeed(seed, 69696969)));
    mountainMask.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mountainMask.SetFrequency(0.0015f);

    terrainNoise.SetSeed(static_cast<int>(seed));
    terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrainNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    terrainNoise.SetFractalOctaves(4);
    terrainNoise.SetFractalLacunarity(2.0f);
    terrainNoise.SetFractalGain(0.5f);
    terrainNoise.SetFrequency(0.008f);

    treeNoise.SetSeed(static_cast<int>(DeriveSeed(seed, 67676767)));
    treeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    treeNoise.SetFrequency(0.007f);

    caveNoise.SetSeed(static_cast<int>(seed + 1337));
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    caveNoise.SetFrequency(0.03f);

    caveRegionNoise.SetSeed(static_cast<int>(DeriveSeed(seed, 12345678)));
    caveRegionNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    caveRegionNoise.SetFrequency(0.3f);

    caveEntranceNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    caveEntranceNoise.SetFrequency(0.02f);
}

int WorldGen::GetTerrainHeight(const int worldX, const int worldZ) {
    float m = mountainMask.GetNoise(static_cast<float>(worldX),
                                    static_cast<float>(worldZ));
    m = (m + 1.0f) * 0.5f;

    float p = peakNoise.GetNoise(static_cast<float>(worldX),
                                 static_cast<float>(worldZ));
    p = (p + 1.0f) * 0.5f;
    p = std::pow(p, 6.0f);
    float peakFactor = m * p;

    float n = terrainNoise.GetNoise(static_cast<float>(worldX),
                                    static_cast<float>(worldZ));

    constexpr float amplitude = 12;
    constexpr float mountainAmplitude = 50;
    constexpr float peakAmplitude = 180;

    int height = baseMapHeight +
                 static_cast<int>((n * amplitude) + (m * mountainAmplitude) +
                                  (peakFactor * peakAmplitude));

    return std::clamp(height, minMapHeight, maxMapHeight);
}

void GenTreeStem(const glm::ivec3 &treePos, Chunk &chunk) {
    for (int y = 0; y < 4; y++) {
        glm::ivec3 stemPos(treePos.x, treePos.y + y, treePos.z);
        chunk.SetBlock(stemPos, BlockType::OAK_LOG);
    }
}
void GenTreeLeaves(const glm::ivec3 &treePos, Chunk &chunk) {
    for (int x = -1; x < 2; x++) {
        for (int y = 2; y < 5; y++) {
            for (int z = -1; z < 2; z++) {
                if (x == 0 && y < 4 && z == 0)
                    continue;
                glm::ivec3 leavePos(treePos.x + x, treePos.y + y,
                                    treePos.z + z);
                chunk.SetBlock(leavePos, BlockType::OAK_LEAVES);
            }
        }
    }
}
void GenTree(const glm::ivec3 &treePos, Chunk &chunk) {
    GenTreeStem(treePos, chunk);
    GenTreeLeaves(treePos, chunk);
}

void WorldGen::GenCaves(const int x, const int z, const glm::ivec3 &world,
                        const int height, Chunk &chunk) {
    if (world.y > height - 4) {
        float entrance =
            caveEntranceNoise.GetNoise(static_cast<float>(world.x) * 0.02f,
                                       static_cast<float>(world.z) * 0.02f);
        if (entrance < 0.6f)
            return;
    }

    if (world.y > 0) {

        float region =
            caveRegionNoise.GetNoise(static_cast<float>(world.x) * 0.05f,
                                     static_cast<float>(world.z) * 0.05f);

        if (region < 0.5f)
            return;

        constexpr float caveScale = 0.5f;

        float n = caveNoise.GetNoise(static_cast<float>(world.x) * caveScale,
                                     static_cast<float>(world.y) * caveScale,
                                     static_cast<float>(world.z) * caveScale);
        float tunnel = 1.0f - std::abs(n);

        constexpr float maxCaveY = 1000.0f;

        float depth = 1.0f - (static_cast<float>(world.y) / maxCaveY);
        depth = std::clamp(depth, 0.2f, 1.0f);

        if (tunnel * depth > 0.7f)
            chunk.SetBlock(glm::ivec3(x, world.y, z), BlockType::AIR);
    }
}

void WorldGen::GenTrees(const int x, const int z, const int worldX,
                        const int worldZ, const int height, Chunk &chunk) {
    float t = treeNoise.GetNoise(static_cast<float>(worldX),
                                 static_cast<float>(worldZ));

    Block block = chunk.GetBlock(glm::ivec3(x, height, z));

    if (RandPercentFloat(4.0f * t) && block.IsSolid() &&
        block.type != BlockType::SNOW)
        GenTree(glm::ivec3(x, height + 1, z), chunk);
}

void WorldGen::GenMap() {
    for (int x = -viewDist; x < viewDist; x++) {
        for (int z = -viewDist; z < viewDist; z++) {
            manager.RequestChunkGen(glm::ivec3(x, 0, z), this);
        }
    }
}

void WorldGen::UpdateMap() {
    const glm::ivec3 playerChunkPos = ChunkManager::GetPlayerChunkPos(GetCam3D().position);
    if (manager.lastPlayerChunkPos == playerChunkPos)
        return;

    for (int x = playerChunkPos.x - viewDist; x < playerChunkPos.x + viewDist; x++) {
        for (int z = playerChunkPos.z - viewDist; z < playerChunkPos.z + viewDist; z++) {
            manager.RequestChunkGen(glm::ivec3(x, 0, z), this);
        }
    }
}
