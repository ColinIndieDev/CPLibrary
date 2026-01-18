#pragma once

#include "Chunk.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>

class WorldGen;

struct IVec3Hash {
    std::size_t operator()(const glm::ivec3 &v) const {
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        std::size_t h3 = std::hash<int>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct IVec3Equal {
    bool operator()(const glm::ivec3 &a, const glm::ivec3 &b) const {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

struct ChunkGenRequest {
    glm::ivec3 position;
    WorldGen *worldGen;
    int priority;
};

struct DirtyGenRequest {
    Chunk *chunk;
    int priority;

    DirtyGenRequest(Chunk *chunk, int priority)
        : chunk(chunk), priority(priority) {}
};

struct RequestCompare {
    bool operator()(const ChunkGenRequest &a, const ChunkGenRequest &b) {
        return a.priority > b.priority;
    }
};

struct ChunkPriorCompare {
    bool operator()(const DirtyGenRequest &a, const DirtyGenRequest &b) {
        return a.priority > b.priority;
    }
};

class ChunkManager {
  public:
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash, IVec3Equal> chunks;
    std::vector<Chunk *> visibleChunks;
    glm::ivec3 lastPlayerChunkPos;

    ChunkManager(uint32_t threadCount = std::thread::hardware_concurrency() /
                                        2);
    ~ChunkManager();

    ChunkManager(const ChunkManager &) = delete;
    ChunkManager &operator=(const ChunkManager &) = delete;

    ChunkManager(ChunkManager &&other) noexcept = delete;
    ChunkManager &operator=(ChunkManager &&other) noexcept = delete;

    Block GetBlockGlobal(const glm::ivec3 &worldPos);
    Chunk *GetChunk(const glm::ivec3 &chunkPos);
    static glm::ivec3 GetPlayerChunkPos(const glm::vec3 &playerPos);
    static bool OutOfRenderDist(const glm::ivec3 &chunkPos, int viewDist);

    void RequestChunkGen(const glm::ivec3 &chunkPos, WorldGen *worldGen);
    void ProcessFinishedChunks();
    void ProcessDirtyChunks(int viewDist, int maxPerFrame = 10);
    void UploadChunkMeshes();
    void MarkChunkDirty(const glm::ivec3 &chunkPos);
    void DestroyBlock(const glm::ivec3 &worldPos);
    void PlaceBlock(const glm::ivec3 &worldPos, const BlockType &type);
    bool IsGenComplete() const;
    void UpdateVisibleChunks(int viewDist);
    void UpdateVisibleChunksDepth(int viewDist);
    uint32_t
    DrawShadowMapChunks(ShadowMap *shadowMap, const glm::mat4 &lightSpaceMatrix,
                        const std::map<BlockType, Texture2D *> &atlases,
                        bool useShadows);
    uint32_t DrawChunks(const Shader &shader, const Shader &depthShader,
                        const std::map<BlockType, Texture2D *> &atlases);
    uint32_t
    DrawTransparentChunks(const Shader &shader,
                          const std::map<BlockType, Texture2D *> &atlases);
    void Stop();

    std::mutex m_ChunksMutex; // Temporary for debugging
  private:
    void m_WorkerThread();
    Chunk m_GenChunk(const glm::ivec3 &pos, WorldGen *worldGen);

    std::priority_queue<ChunkGenRequest, std::vector<ChunkGenRequest>,
                        RequestCompare>
        m_RequestQueue;
    std::priority_queue<DirtyGenRequest, std::vector<DirtyGenRequest>,
                        ChunkPriorCompare>
        m_DirtyQueue;
    std::unordered_set<glm::ivec3, IVec3Hash, IVec3Equal> m_DirtyList;
    std::unordered_set<glm::ivec3, IVec3Hash, IVec3Equal> m_UnfinishedList;
    std::queue<std::pair<glm::ivec3, Chunk>> m_FinishedQueue;
    std::mutex m_RequestMutex;
    std::mutex m_FinishedMutex;
    std::condition_variable m_RequestCV;
    std::vector<std::thread> m_Workers;
    std::atomic<bool> m_Running{true};
    std::atomic<int> m_PendingChunks{0};
};
