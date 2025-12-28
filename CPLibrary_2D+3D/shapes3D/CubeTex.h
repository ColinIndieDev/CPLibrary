#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;
class Texture2D;

class CubeTex {
  public:
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 textureSize;
    int channels{};
    Color color;
    unsigned int texture{};

    explicit CubeTex(glm::vec3 pos, glm::vec3 size, Color color);
    ~CubeTex();

    CubeTex(const CubeTex &) = delete;
    CubeTex &operator=(const CubeTex &) = delete;

    CubeTex(CubeTex &&other) noexcept
        : position(other.position), size(other.size),
          textureSize(other.textureSize), channels(other.channels),
          color(other.color), texture(other.texture), VBO(other.VBO),
          VAO(other.VAO) {
        other.VBO = 0;
        other.VAO = 0;
        other.texture = 0;
    }

    CubeTex &operator=(CubeTex &&other) noexcept {
        if (this != &other) {
            if (VAO != 0 && glIsVertexArray(VAO)) {
                glDeleteVertexArrays(1, &VAO);
            }
            if (VBO != 0 && glIsBuffer(VBO)) {
                glDeleteBuffers(1, &VBO);
            }

            position = other.position;
            size = other.size;
            textureSize = other.textureSize;
            color = other.color;
            channels = other.channels;
            texture = other.texture;

            VBO = other.VBO;
            VAO = other.VAO;
            other.VBO = 0;
            other.VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, const Texture2D* texture) const;
    void DrawDepth(const Shader &shader, const Texture2D* texture) const;
  private:
    unsigned int VBO{}, VAO{};
};
} // namespace CPL
