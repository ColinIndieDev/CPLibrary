#include "Tilemap.h"
#include "../Shader.h"
#include "Texture2D.h"
#include <algorithm>

namespace CPL {
Tilemap::Tilemap() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindVertexArray(0);
}

void Tilemap::BeginEditing() { batches.clear(); }

void Tilemap::AddTile(const glm::vec2 position, const glm::vec2 size,
                      const Texture2D *texture) {
    if (!texture || texture->texture == 0)
        return;
    float quad[] = {position.x,          position.y,          0, 0, 1,
                    position.x + size.x, position.y,          0, 1, 1,
                    position.x + size.x, position.y + size.y, 0, 1, 0,

                    position.x,          position.y,          0, 0, 1,
                    position.x + size.x, position.y + size.y, 0, 1, 0,
                    position.x,          position.y + size.y, 0, 0, 0};
    auto &[vertices, VBO] = batches[texture->texture];
    vertices.insert(vertices.end(), std::begin(quad), std::end(quad));
    tiles.push_back(Tile(position, size));
    if (VBO == 0)
        glGenBuffers(1, &VBO);
}

void Tilemap::DeleteTile(glm::vec2 position, glm::vec2 size,
                         const Texture2D *texture) {
    if (!texture || texture->texture == 0)
        return;
    std::erase_if(tiles, [&](const Tile &tile) {
        return tile.pos == position && tile.size == size;
    });

    auto it = batches.find(texture->texture);
    if (it == batches.end())
        return;
    auto &[vertices, VBO] = it->second;

    // 6 vertices * 5 floats per vertice (x, y, z + x & y of texture)
    const int floatsPerQuad = 30;

    for (int i = 0; i < vertices.size(); i += floatsPerQuad) {
        if (i + 1 < vertices.size()) {
            const float quadX = vertices[i];
            const float quadY = vertices[i + 1];

            if (std::abs(quadX - position.x) < 0.01f &&
                std::abs(quadY - position.y) < 0.01f) {
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

bool Tilemap::TileExist(const glm::vec2 position, const glm::vec2 size) {
    Tile tile = Tile(position, size);
    auto it = std::find(tiles.begin(), tiles.end(), tile);
    return it != tiles.end();
}

void Tilemap::CheckCollidableTiles(const float size) {
    for (auto &tile : tiles) {
        glm::vec2 p = tile.pos / size;
        bool exposed = false;

        std::array<glm::vec2, 4> neighbors = {
            glm::vec2(p.x - 1, p.y), glm::vec2(p.x + 1, p.y),
            glm::vec2(p.x, p.y - 1), glm::vec2(p.x, p.y + 1)};

        for (auto &n : neighbors) {
            if (!TileExist({n.x * size, n.y * size}, {size, size})) {
                exposed = true;
                break;
            }
        }
        tile.isCollidable = exposed;
    }
}

void Tilemap::Draw() {
    constexpr auto transform = glm::mat4(1.0f);

    if (currentDrawMode == TEXTURE_LIGHT) {
        lightTextureShader.SetMatrix4fv("transform", transform);
        lightTextureShader.SetVector3f("offset",
                                       glm::vec3(glm::vec2(0, 0), 0.0f));
        lightTextureShader.SetColor("inputColor", WHITE);
    } else {
        textureShader.SetMatrix4fv("transform", transform);
        textureShader.SetVector3f("offset", glm::vec3(glm::vec2(0, 0), 0.0f));
        textureShader.SetColor("inputColor", WHITE);
    }

    glBindVertexArray(VAO);

    for (auto &[id, batch] : batches) {
        glBindBuffer(GL_ARRAY_BUFFER, batch.VBO);
        glBufferData(GL_ARRAY_BUFFER, batch.vertices.size() * sizeof(float),
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
