#pragma once
#include <vector>
#include "../CPL.h"

namespace CPL {
    class Shader;
    class Texture2D;

    class Tilemap {
    public:
        struct TileBatch {
            std::vector<float> vertices;
            unsigned int VBO = 0;
        };
	struct Tile {
	    Tile(const glm::vec2 pos, const glm::vec2 size) : pos(pos), size(size) {}
            glm::vec2 pos;
            glm::vec2 size;
            bool isCollidable = false;

	    bool operator==(const Tile& other) const {
                return pos == other.pos && size == other.size;
            }
        };

	std::vector<Tile> tiles;
        std::unordered_map<unsigned int, TileBatch> batches;

        Tilemap();
        void BeginEditing();
        void AddTile(glm::vec2 position, glm::vec2 size, const Texture2D* texture);
	bool TileExist(glm::vec2 position, glm::vec2 size);
	void CheckCollidableTiles(float size);
        void Draw(const Shader& shader);
    private:
        unsigned int VAO{};
    };
}
