#pragma once
#include "../CPL.h"
#include <vector>

namespace CPL {
class Shader;
class Texture2D;

class Tilemap {
  public:
    struct TileBatch {
        std::vector<float> vertices;
        uint32_t VBO = 0;
    };
    struct Tile {
        Tile(const glm::vec2 pos, const glm::vec2 size)
            : pos(pos), size(size) {}
        glm::vec2 pos;
        glm::vec2 size;
        bool isCollidable = false;

        bool operator==(const Tile &other) const {
            return pos == other.pos && size == other.size;
        }
    };

    std::vector<Tile> tiles;
    std::unordered_map<uint32_t, TileBatch> batches;

    Tilemap();
    void BeginEditing();
    void AddTile(const glm::vec2 &pos, const glm::vec2 &size, const Texture2D *tex);
    void DeleteTile(const glm::vec2 &pos, const glm::vec2 &size,
                    const Texture2D * tex);
    bool TileExist(const glm::vec2 &pos, const glm::vec2 &size);
    void CheckCollidableTiles(const glm::vec2 &size);
    void Draw();

  private:
    uint32_t m_VAO{};
};
} // namespace CPL
