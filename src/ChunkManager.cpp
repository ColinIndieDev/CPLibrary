#include "ChunkManager.h"
#include "WorldGen.h"
#include <iterator>
#include <string>

int FloorDiv(const int x, const int y) {
    return (x / y) - (x % y != 0 && (x ^ y) < 0 ? 1 : 0);
}

int PositiveMod(const int x, const int y) {
    const int result = x % y;
    return result < 0 ? result + y : result;
}

ChunkManager::ChunkManager(const int threadCount) {
    for (int i = 0; i < threadCount; i++) {
        m_Workers.emplace_back(&ChunkManager::m_WorkerThread, this);
    }

    Logging::Log(Logging::MessageStates::INFO, "Initializing " + std::to_string(threadCount) + " threads!");
}

ChunkManager::~ChunkManager() { Stop(); }

void ChunkManager::Stop() {
    m_Running = false;
    m_RequestCV.notify_all();

    for (auto &worker : m_Workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ChunkManager::RequestChunkGen(const glm::ivec3 &pos, WorldGen *worldGen) {
    {
        std::lock_guard<std::mutex> lock(m_RequestMutex);
        m_RequestQueue.push({pos, worldGen});
        m_PendingChunks++;
    }
    m_RequestCV.notify_one();
}

void ChunkManager::ProcessFinishedChunks() {
    std::lock_guard<std::mutex> lock(m_FinishedMutex);

    while (!m_FinishedQueue.empty()) {
        auto [pos, chunk] = std::move(m_FinishedQueue.front());
        m_FinishedQueue.pop();
        {
            std::lock_guard<std::mutex> chunkLock(m_ChunksMutex);
            chunk.GenMeshGL();
            chunks.emplace(pos, std::move(chunk));        
        }
    }
}

bool ChunkManager::IsGenComplete() const {
    return m_PendingChunks == 0 && m_FinishedQueue.empty();
}

void ChunkManager::DrawChunks(const Shader &shader,
                              const std::map<BlockType, Texture2D *> &atlases) {
    std::lock_guard<std::mutex> lock(m_ChunksMutex);

    for (auto &[pos, chunk] : chunks) {
        const float blockSize = 0.2f;
        const float halfSizeXZ = Chunk::s_Size * blockSize * 0.5f;
        const float halfSizeY = Chunk::s_Height * blockSize * 0.5f;
        const glm::vec3 center(glm::vec3(pos) * halfSizeXZ * 2.0f +
                               glm::vec3(halfSizeXZ, halfSizeY, halfSizeXZ));
        const glm::vec3 halfSize(halfSizeXZ, halfSizeY, halfSizeXZ);

        if (GetCam3D().frustum.IsCubeVisible(center, halfSize)) {
            chunk.Draw(shader, atlases);
        }
    }
}

void ChunkManager::m_WorkerThread() {
    while (m_Running) {
        ChunkGenRequest request{};
        {
            std::unique_lock<std::mutex> lock(m_RequestMutex);

            m_RequestCV.wait(
                lock, [this] { return !m_RequestQueue.empty() || !m_Running; });

            if (!m_Running)
                break;
            if (m_RequestQueue.empty())
                continue;

            request = m_RequestQueue.front();
            m_RequestQueue.pop();
        }
        Chunk chunk = m_GenChunk(request.position, request.worldGen);
        chunk.GenMesh(*this);

        {
            std::lock_guard<std::mutex> lock(m_FinishedMutex);
            m_FinishedQueue.emplace(request.position, std::move(chunk));
            m_PendingChunks--;
        }
    }
}

Chunk ChunkManager::m_GenChunk(const glm::ivec3 &pos, WorldGen *worldGen) {
    Chunk chunk(pos);

    for (int z = 0; z < Chunk::s_Size; z++) {
        for (int x = 0; x < Chunk::s_Size; x++) {
            int worldX = (pos.x * 16) + x;
            int worldZ = (pos.z * 16) + z;

            int height = worldGen->GetTerrainHeight(worldX, worldZ);

            for (int y = 0; y <= height; y++) {
                if (y == 0)
                    chunk.SetBlock(glm::ivec3(x, y, z), BlockType::BEDROCK);
                else if (y == height && height < 100)
                    chunk.SetBlock(glm::ivec3(x, y, z), BlockType::GRASS_BLOCK);
                else if (y < height && y > height - 4)
                    chunk.SetBlock(glm::ivec3(x, y, z), BlockType::DIRT);
                else if (y <= height - 4 || (y <= height && y >= 100 &&
                                             height >= 100 && height < 170))
                    chunk.SetBlock(glm::ivec3(x, y, z), BlockType::STONE);
                else if (y <= height && y >= 170 && height >= 170)
                    chunk.SetBlock(glm::ivec3(x, y, z), BlockType::SNOW);

                worldGen->GenCaves(x, z, glm::ivec3(worldX, y, worldZ), height,
                                   chunk);
            }

            worldGen->GenTrees(x, z, worldX, worldZ, height, chunk);
        }
    }
    return chunk;
}

Chunk *ChunkManager::GetChunk(const glm::ivec3 &chunkPos) {
    std::lock_guard<std::mutex> lock(m_ChunksMutex);

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
