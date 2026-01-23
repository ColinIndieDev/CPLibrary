#pragma once

#include <array>
#include <glm/glm.hpp>

enum class BlockType : uint8_t {
    AIR,
    BEDROCK,
    GRASS_BLOCK,
    DIRT,
    STONE,
    OAK_LOG,
    OAK_LEAVES,
    SNOW,
    SAND,
    WATER,
    GRASS
};

enum class FaceDirection : uint8_t { BACK, FRONT, LEFT, RIGHT, BOTTOM, TOP };

struct FaceVertices {
    std::array<glm::vec3, 6> pos;
    glm::vec3 normal;
    std::array<glm::vec2, 6> uvs;

    FaceVertices(const std::array<glm::vec3, 6> &pos, const glm::vec3 &normal,
                 const std::array<glm::vec2, 6> &uvs)
        : pos(pos), normal(normal), uvs(uvs) {}
};

class FaceData {
  public:
    static const std::array<FaceVertices, 6> s_Faces;
    static const std::array<FaceVertices, 2> s_FoliageFaces;
};

struct Block {
    BlockType type = BlockType::AIR;

    [[nodiscard]] bool IsSolid() const { return type != BlockType::AIR; }

    [[nodiscard]] bool IsFluid() const { return type == BlockType::WATER; }

    [[nodiscard]] bool IsOpaque() const {
        return type != BlockType::AIR && type != BlockType::OAK_LEAVES &&
               type != BlockType::WATER;
    }

    [[nodiscard]] bool IsUnbreakable() const {
        return type == BlockType::BEDROCK;
    }
};
