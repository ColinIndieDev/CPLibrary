#include "Chunk.h"
#include "Block.h"
#include "ChunkManager.h"
#include <iterator>

Chunk::Chunk(const glm::ivec3 &chunkPos)
    : state(MeshState::NONE), needUpload(false), m_Pos(chunkPos) {
    m_Blocks.resize(static_cast<int>(s_Size * s_Height * s_Size),
                    Block{BlockType::AIR});
}

Chunk::~Chunk() {
    for (auto &[type, mesh] : m_Meshes) {
        if (mesh.VAO != 0)
            glDeleteVertexArrays(1, &mesh.VAO);
        if (mesh.VBO != 0)
            glDeleteBuffers(1, &mesh.VBO);
    }
}

void Chunk::MarkDirty() {
    if (state == MeshState::READY || state == MeshState::MESHED_LOCAL) {
        state = MeshState::DIRTY;
    }
}

void Chunk::SetBlock(const glm::ivec3 &pos, const BlockType &type) {
    if (pos.x < 0 || pos.x >= s_Size || pos.y < 0 || pos.y >= s_Height ||
        pos.z < 0 || pos.z >= s_Size)
        return;
    m_Blocks[m_GetIndex(pos)].type = type;
}

Block Chunk::GetBlock(const glm::ivec3 &pos) const {
    if (pos.x < 0 || pos.x >= s_Size || pos.y < 0 || pos.y >= s_Height ||
        pos.z < 0 || pos.z >= s_Size)
        return Block{BlockType::AIR};
    return m_Blocks[m_GetIndex(pos)];
}

glm::ivec3 Chunk::GetPos() { return m_Pos; }

int Chunk::m_GetIndex(const glm::ivec3 &pos) {
    return pos.x + (pos.z * s_Size) + ((pos.y * s_Size) * s_Size);
}

bool Chunk::m_RenderFace(const glm::ivec3 &pos, const FaceDirection &face,
                         ChunkManager &manager, const bool localOnly) const {
    Block block = GetBlock(pos);
    if (!block.IsSolid())
        return false;

    static const std::array<glm::vec3, 6> offsets = {{
        {0, 0, -1}, // Back face
        {0, 0, 1},  // Front face
        {-1, 0, 0}, // Left face
        {1, 0, 0},  // Right face
        {0, -1, 0}, // Bottom face
        {0, 1, 0}   // Top face
    }};

    glm::ivec3 offset = offsets.at(static_cast<int>(face));
    glm::ivec3 neighborGridPos = pos + offset;
    Block neighbor;

    if (neighborGridPos.x >= 0 && neighborGridPos.x < s_Size &&
        neighborGridPos.y >= 0 && neighborGridPos.y < s_Height &&
        neighborGridPos.z >= 0 && neighborGridPos.z < s_Size) {
        neighbor = GetBlock(neighborGridPos);
    } else {
        if (localOnly)
            return false;

        glm::ivec3 worldPos =
            m_Pos * glm::ivec3(s_Size, s_Height, s_Size) + neighborGridPos;
        neighbor = manager.GetBlockGlobal(worldPos);
    }

    return !neighbor.IsSolid() || (!neighbor.IsOpaque() && block.IsOpaque());
}

void Chunk::m_AddFaceToMesh(const BlockType &type, const glm::vec3 &worldPos,
                            const FaceDirection &face) {
    const FaceVertices &faceData = FaceData::s_Faces.at(static_cast<int>(face));

    MeshBatch &batch = m_Meshes[type];

    for (int i = 0; i < 6; i++) {
        glm::vec3 pos = worldPos + faceData.pos.at(i);
        glm::vec2 uv = faceData.uvs.at(i);

        batch.vertices.push_back(pos.x);
        batch.vertices.push_back(pos.y);
        batch.vertices.push_back(pos.z);

        batch.vertices.push_back(faceData.normal.x);
        batch.vertices.push_back(faceData.normal.y);
        batch.vertices.push_back(faceData.normal.z);

        batch.vertices.push_back(uv.x);
        batch.vertices.push_back(uv.y);
    }
}

void Chunk::GenMesh(ChunkManager &manager, const bool localOnly) {
    for (auto &[type, mesh] : m_Meshes) {
        mesh.vertices.clear();
    }

    for (int y = 0; y < s_Height; y++) {
        for (int z = 0; z < s_Size; z++) {
            for (int x = 0; x < s_Size; x++) {
                const float blockSize = 0.2f;

                Block block = GetBlock(glm::vec3(x, y, z));
                if (!block.IsSolid())
                    continue;

                glm::vec3 worldPos((static_cast<float>((m_Pos.x * s_Size)) +
                                    static_cast<float>(x)) *
                                       blockSize,
                                   (static_cast<float>((m_Pos.y * s_Height)) +
                                    static_cast<float>(y)) *
                                       blockSize,
                                   (static_cast<float>((m_Pos.z * s_Size)) +
                                    static_cast<float>(z)) *
                                       blockSize);

                for (int f = 0; f < 6; f++) {
                    auto face = static_cast<FaceDirection>(f);
                    if (m_RenderFace(glm::vec3(x, y, z), face, manager,
                                     localOnly)) {
                        m_AddFaceToMesh(block.type, worldPos, face);
                    }
                }
            }
        }
    }
}

void Chunk::GenMeshGL() {
    for (auto &[type, mesh] : m_Meshes) {
        if (mesh.vertices.empty())
            continue;

        mesh.vertexCount = static_cast<int>(mesh.vertices.size()) / 8;

        if (mesh.VAO == 0) {
            glGenVertexArrays(1, &mesh.VAO);
            glGenBuffers(1, &mesh.VBO);
        }

        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<int>(mesh.vertices.size() * sizeof(float)),
                     mesh.vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              static_cast<void *>(nullptr));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              reinterpret_cast<void *>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void Chunk::Draw(const Shader &shader,
                 const std::map<BlockType, Texture2D *> &atlases) {
    auto transform = glm::mat4(1.0f);

    glm::mat4 view = GetCam3D().GetViewMatrix();
    glm::mat4 projection =
        GetCam3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight());
    shader.SetMatrix4fv("projection", projection * view);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(0));
    shader.SetColor("inputColor", WHITE);
    shader.SetInt("ourTexture", 0);

    for (auto &[type, mesh] : m_Meshes) {
        if (type == BlockType::WATER)
            continue;

        if (mesh.vertexCount == 0)
            continue;

        if (type == BlockType::OAK_LEAVES) {
            shader.SetColor("inputColor", Color(102, 200, 45, 255));
        } else {
            shader.SetColor("inputColor", WHITE);
        }

        auto it = atlases.find(type);
        if (it == atlases.end())
            continue;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, it->second->tex);

        glBindVertexArray(mesh.VAO);
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    }

    glBindVertexArray(0);
}

void Chunk::DrawTransparent(const Shader &shader,
                            const std::map<BlockType, Texture2D *> &atlases) {
    auto transform = glm::mat4(1.0f);

    glm::mat4 view = GetCam3D().GetViewMatrix();
    glm::mat4 projection =
        GetCam3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight());
    shader.SetMatrix4fv("projection", projection * view);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(0));
    shader.SetColor("inputColor", WHITE);
    shader.SetInt("ourTexture", 0);

    if (m_Meshes[BlockType::WATER].vertexCount == 0) {
        glBindVertexArray(0);
        return;
    }

    auto it = atlases.find(BlockType::WATER);
    if (it == atlases.end()) {
        glBindVertexArray(0);
        return;
    }

    shader.SetColor("inputColor", Color(0, 100, 255, 150));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, it->second->tex);

    glBindVertexArray(m_Meshes[BlockType::WATER].VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_Meshes[BlockType::WATER].vertexCount);

    glBindVertexArray(0);
}

void Chunk::DrawDepth(const Shader &shader,
                      const std::map<BlockType, Texture2D *> &atlases) {
    auto transform = glm::mat4(1.0f);

    glm::mat4 view = GetCam3D().GetViewMatrix();
    glm::mat4 projection =
        GetCam3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight());
    shader.SetMatrix4fv("projection", projection * view);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(0));
    shader.SetInt("ourTexture", 0);

    for (auto &[type, mesh] : m_Meshes) {
        if (type == BlockType::WATER)
            continue;

        if (mesh.vertexCount == 0)
            continue;

        auto it = atlases.find(type);
        if (it == atlases.end())
            continue;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, it->second->tex);

        glBindVertexArray(mesh.VAO);
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    }
    glBindVertexArray(0);
}

void Chunk::DrawDepthShadow(const Shader &shader,
                            const std::map<BlockType, Texture2D *> &atlases) {
    auto model = glm::mat4(1.0f);

    shader.Use();
    shader.SetMatrix4fv("model", model);
    shader.SetInt("ourTexture", 0);

    for (auto &[type, mesh] : m_Meshes) {
        if (mesh.vertexCount == 0)
            continue;

        auto it = atlases.find(type);
        if (it == atlases.end())
            continue;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, it->second->tex);

        glBindVertexArray(mesh.VAO);
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    }
    glBindVertexArray(0);
}
