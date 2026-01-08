#pragma once

#include "Chunk.h"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>

class WorldGen;

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

struct ChunkGenRequest {
    glm::ivec3 position;
    WorldGen* worldGen;
};

class ChunkManager {
  public:
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash, IVec3Equal> chunks;

    ChunkManager(int threadCount = 4);
    ~ChunkManager();

    Block GetBlockGlobal(const glm::ivec3 &worldPos);
    Chunk *GetChunk(const glm::ivec3 &chunkPos);

    void RequestChunkGen(const glm::ivec3 &chunkPos, WorldGen* worldGen);
    void ProcessFinishedChunks();
    bool IsGenComplete() const;
    void DrawChunks(const Shader& shader, const std::map<BlockType, Texture2D*>& atlases);
    void Stop();

  private:
    void m_WorkerThread();
    static Chunk m_GenChunk(const glm::ivec3 &pos, WorldGen* worldGen);

    std::queue<ChunkGenRequest> m_RequestQueue;
    std::queue<std::pair<glm::ivec3, Chunk>> m_FinishedQueue;
    std::mutex m_RequestMutex;
    std::mutex m_FinishedMutex;
    std::mutex m_ChunksMutex;
    std::condition_variable m_RequestCV;
    std::vector<std::thread> m_Workers;
    std::atomic<bool> m_Running{true};
    std::atomic<int> m_PendingChunks{0};
};
