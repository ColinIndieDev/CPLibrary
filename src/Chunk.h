#pragma once

#include "../CPLibrary/CPLibrary.h"

#include "Block.h"
#include <map>
#include <vector>

using namespace CPL;

class ChunkManager;

class Chunk {
  public:
    enum class MeshState : uint8_t {
        NONE,
        MESHED_LOCAL,
        DIRTY,
        READY,
    };

    static constexpr int s_Size = 16;
    static constexpr int s_Height = 256;

    MeshState state;
    bool needUpload;

    Chunk(const glm::ivec3 &chunkPos);
    ~Chunk();

    Chunk(const Chunk &) = delete;
    Chunk &operator=(const Chunk &) = delete;

    Chunk(Chunk &&other) noexcept
        : state(other.state), needUpload(other.needUpload), m_Pos(other.m_Pos),
          m_Blocks(std::move(other.m_Blocks)),
          m_Meshes(std::move(other.m_Meshes)) {
        for (auto &[type, mesh] : other.m_Meshes) {
            mesh.VAO = 0;
            mesh.VBO = 0;
            mesh.vertexCount = 0;
            mesh.vertices.clear();
        }
    }

    Chunk &operator=(Chunk &&other) noexcept {
        if (this != &other) {
            for (auto &[type, mesh] : m_Meshes) {
                if (mesh.VAO != 0)
                    glDeleteVertexArrays(1, &mesh.VAO);
                if (mesh.VBO != 0)
                    glDeleteBuffers(1, &mesh.VBO);
            }

            state = other.state;
            needUpload = other.needUpload;
            m_Pos = other.m_Pos;
            m_Blocks = std::move(other.m_Blocks);
            m_Meshes = std::move(other.m_Meshes);

            for (auto &[type, mesh] : other.m_Meshes) {
                mesh.VAO = 0;
                mesh.VBO = 0;
                mesh.vertexCount = 0;
                mesh.vertices.clear();
            }
        }
        return *this;
    }

    void MarkDirty();

    void SetBlock(const glm::ivec3 &pos, const BlockType &type);
    [[nodiscard]] Block GetBlock(const glm::ivec3 &pos) const;
    glm::ivec3 GetPos();

    void GenMesh(ChunkManager &manager, bool localOnly);
    void GenMeshGL();

    void Draw(const Shader &shader,
              const std::map<BlockType, Texture2D *> &atlases);
    void DrawDepth(const Shader &shader,
                   const std::map<BlockType, Texture2D *> &atlases);
    void DrawDepthShadow(const Shader &shader,
                   const std::map<BlockType, Texture2D *> &atlases);

  private:
    struct MeshBatch {
        std::vector<float> vertices;
        uint32_t VAO = 0;
        uint32_t VBO = 0;
        int vertexCount = 0;
    };

    glm::ivec3 m_Pos;
    std::vector<Block> m_Blocks;

    std::map<BlockType, MeshBatch> m_Meshes;

    [[nodiscard]] static int m_GetIndex(const glm::ivec3 &pos);
    [[nodiscard]] bool m_RenderFace(const glm::ivec3 &pos,
                                    const FaceDirection &face,
                                    ChunkManager &manager,
                                    bool localOnly) const;
    void m_AddFaceToMesh(const BlockType &type, const glm::vec3 &worldPos,
                         const FaceDirection &face);
};
