#include "ChunkManager.h"

int FloorDiv(const int x, const int y) {
    return (x / y) - (x % y != 0 && (x ^ y) < 0 ? 1 : 0);
}

int PositiveMod(const int x, const int y) {
    const int result = x % y;
    return result < 0 ? result + y : result;
}

Chunk *ChunkManager::GetChunk(const glm::ivec3 &chunkPos) {
    auto it = chunks.find(chunkPos);
    if (it != chunks.end()) {
        return &it->second;
    }
    return nullptr;
}

Block ChunkManager::GetBlockGlobal(const glm::ivec3 &worldPos) {
    glm::ivec3 chunkPos(FloorDiv(worldPos.x, Chunk::s_Size),
                        FloorDiv(worldPos.y, Chunk::s_Height),
                        FloorDiv(worldPos.z, Chunk::s_Size));

    Chunk *chunk = GetChunk(chunkPos);
    if (!static_cast<bool>(chunk))
        return Block{BlockType::AIR};

    glm::ivec3 localPos(PositiveMod(worldPos.x, Chunk::s_Size),
                        PositiveMod(worldPos.y, Chunk::s_Height),
                        PositiveMod(worldPos.z, Chunk::s_Size));

    if (localPos.x < 0)
        localPos.x += Chunk::s_Size;
    if (localPos.y < 0)
        localPos.y += Chunk::s_Height;
    if (localPos.z < 0)
        localPos.z += Chunk::s_Size;

    return chunk->GetBlock(localPos);
}
