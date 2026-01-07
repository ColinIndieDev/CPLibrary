#include "../../include/shape2D/Tilemap.h"
#include "../../include/Shader.h"
#include "../../include/shape2D/Texture2D.h"
#include <algorithm>

namespace CPL {
Tilemap::Tilemap() {
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindVertexArray(0);
}

void Tilemap::BeginEditing() { batches.clear(); }

void Tilemap::AddTile(const glm::vec2 &pos, const glm::vec2 &size,
                      const Texture2D *const tex) {
    if (!static_cast<bool>(tex) || tex->tex == 0)
        return;
    const std::array<float, 30> quad = {
        pos.x,  pos.y,                  0, 0, 1,
        pos.x + size.x, pos.y,          0, 1, 1,
        pos.x + size.x, pos.y + size.y, 0, 1, 0,

        pos.x,          pos.y,          0, 0, 1,
        pos.x + size.x, pos.y + size.y, 0, 1, 0,
        pos.x,          pos.y + size.y, 0, 0, 0
    };

    auto &[vertices, VBO] = batches[tex->tex];
    vertices.insert(vertices.end(), std::begin(quad), std::end(quad));
    tiles.emplace_back(pos, size);
    if (VBO == 0)
        glGenBuffers(1, &VBO);
}

void Tilemap::DeleteTile(const glm::vec2 &pos, const glm::vec2 &size,
                         const Texture2D *const tex) {
    if (!static_cast<bool>(tex) || tex->tex == 0)
        return;
    std::erase_if(tiles, [&](const Tile &tile) {
        return tile.pos == pos && tile.size == size;
    });

    auto it = batches.find(tex->tex);
    if (it == batches.end())
        return;
    auto &[vertices, VBO] = it->second;

    const int floatsPerQuad = 30;

    for (int i = 0; i < vertices.size(); i += floatsPerQuad) {
        if (i + 1 < vertices.size()) {
            const float quadX = vertices[i];
            const float quadY = vertices[i + 1];

            if (std::abs(quadX - pos.x) < 0.01f &&
                std::abs(quadY - pos.y) < 0.01f) {
                vertices.erase(vertices.begin() + i,
                               vertices.begin() + i + floatsPerQuad);
                break;
            }
        }
    }
    if (vertices.empty()) {
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
        }
        batches.erase(it);
    }
}

bool Tilemap::TileExist(const glm::vec2 &pos, const glm::vec2 &size) {
    Tile tile(pos, size);
    auto it = std::ranges::find(tiles.begin(), tiles.end(), tile);
    return it != tiles.end();
}

void Tilemap::CheckCollidableTiles(const glm::vec2 &size) {
    for (auto &tile : tiles) {
        glm::vec2 p = tile.pos / size;
        bool exposed = false;

        std::array<glm::vec2, 4> neighbors = {
            glm::vec2(p.x - 1, p.y), glm::vec2(p.x + 1, p.y),
            glm::vec2(p.x, p.y - 1), glm::vec2(p.x, p.y + 1)
        };

        for (auto &n : neighbors) {
            if (!TileExist(n * size, size)) {
                exposed = true;
                break;
            }
        }
        tile.isCollidable = exposed;
    }
}

void Tilemap::Draw() {
    constexpr auto transform = glm::mat4(1.0f);

    if (GetCurMode() == DrawModes::TEX_LIGHT) {
        GetShader(DrawModes::TEX_LIGHT).SetMatrix4fv("transform", transform);
        GetShader(DrawModes::TEX_LIGHT).SetVector3f("offset",
                                       glm::vec3(glm::vec2(0, 0), 0.0f));
        GetShader(DrawModes::TEX_LIGHT).SetColor("inputColor", WHITE);
    } else {
        GetShader(DrawModes::TEX).SetMatrix4fv("transform", transform);
        GetShader(DrawModes::TEX).SetVector3f("offset", glm::vec3(glm::vec2(0, 0), 0.0f));
        GetShader(DrawModes::TEX).SetColor("inputColor", WHITE);
    }

    glBindVertexArray(m_VAO);

    for (auto &[id, batch] : batches) {
        glBindBuffer(GL_ARRAY_BUFFER, batch.VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(batch.vertices.size() * sizeof(float)),
                     batch.vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              static_cast<void *>(nullptr));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glDrawArrays(GL_TRIANGLES, 0,
                     static_cast<GLsizei>(batch.vertices.size() / 5));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindVertexArray(0);
}
} // namespace CPL
