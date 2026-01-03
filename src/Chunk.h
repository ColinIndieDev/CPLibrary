#pragma once

#include "../CPLibrary/CPLibrary.h"
#include "Block.h"
#include <map>
#include <vector>

using namespace CPL;

class Chunk {
  public:
    static constexpr int s_Size = 16;
    static constexpr int s_Height = 256;

    Chunk(const glm::vec3 &chunkPos);
    ~Chunk();

    void SetBlock(const glm::vec3 &pos, const BlockType &type);
    [[nodiscard]] Block GetBlock(const glm::vec3 &pos) const;

    void GenMesh();

    void Draw(const Shader &shader,
              const std::map<BlockType, Texture2D *> &atlases);

  private:
    struct MeshBatch {
        std::vector<float> vertices;
        uint32_t VAO = 0;
        uint32_t VBO = 0;
        int vertexCount = 0;
    };

    glm::vec3 m_Pos;
    std::vector<Block> m_Blocks;

    std::map<BlockType, MeshBatch> m_Meshes;

    [[nodiscard]] int m_GetIndex(const glm::vec3 &pos) const;
    [[nodiscard]] bool m_RenderFace(const glm::vec3 &pos,
                                    const FaceDirection &face) const;
    void m_AddFaceToMesh(const BlockType &type, const glm::vec3 &worldPos,
                         const FaceDirection &face);
};
