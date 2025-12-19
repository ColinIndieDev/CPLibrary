#pragma once

#include "../CPL.h"

namespace CPL {
struct Color;
class Shader;

class Texture2D {
  public:
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 textureSize;
    int channels{};
    float rotationAngle = 0;
    Color color;
    unsigned int texture{};

    explicit Texture2D(const std::string &filePath, glm::vec2 size,
                       const TextureFiltering &textureFiltering);
    Texture2D(const std::string &filePath, glm::vec2 position, glm::vec2 size,
              const Color &color, const TextureFiltering &textureFiltering);
    ~Texture2D() { Unload(); }

    Texture2D(const Texture2D &) = delete;
    Texture2D &operator=(const Texture2D &) = delete;

    Texture2D(Texture2D &&other) noexcept
        : position(other.position), size(other.size),
          textureSize(other.textureSize), channels(other.channels),
          rotationAngle(other.rotationAngle), color(other.color),
          VBO(other.VBO), VAO(other.VAO), EBO(other.EBO),
          texture(other.texture) {
        other.VBO = 0;
        other.VAO = 0;
        other.EBO = 0;
        other.texture = 0;
    }

    Texture2D &operator=(Texture2D &&other) noexcept {
        if (this != &other) {
            Unload();

            position = other.position;
            size = other.size;
            textureSize = other.textureSize;
            channels = other.channels;
            rotationAngle = other.rotationAngle;
            color = other.color;
            VBO = other.VBO;
            VAO = other.VAO;
            EBO = other.EBO;
            texture = other.texture;

            other.VBO = 0;
            other.VAO = 0;
            other.EBO = 0;
            other.texture = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;
    void Unload() const;

  private:
    unsigned int VBO{}, VAO{}, EBO{};
};
} // namespace CPL
