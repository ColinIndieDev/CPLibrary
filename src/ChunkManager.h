#pragma once

#include "Chunk.h"
#include <unordered_map>

struct IVec3Hash {
    std::size_t operator()(const glm::ivec3& v) const {
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        std::size_t h3 = std::hash<int>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct IVec3Equal {
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

class ChunkManager {
  public:
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash, IVec3Equal> chunks;

    Block GetBlockGlobal(const glm::ivec3 &worldPos);
    Chunk *GetChunk(const glm::ivec3 &chunkPos);
};
