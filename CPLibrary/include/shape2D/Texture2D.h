#pragma once

#include "../CPL.h"

namespace CPL {
struct Color;
class Shader;

class Texture2D {
  public:
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec2 textureSize;
    float rotAngle = 0;
    Color color;
    uint32_t tex{};

    explicit Texture2D(const std::string &filePath, const glm::vec2 &size,
                       const TextureFiltering &textureFiltering);
    Texture2D(const std::string &filePath, const glm::vec2 &pos,
              const glm::vec2 &size, const Color &color,
              const TextureFiltering &textureFiltering);
    ~Texture2D() { m_Unload(); }

    Texture2D(const Texture2D &) = delete;
    Texture2D &operator=(const Texture2D &) = delete;

    Texture2D(Texture2D &&other) noexcept
        : pos(other.pos), size(other.size), textureSize(other.textureSize),
          rotAngle(other.rotAngle), color(other.color), m_VBO(other.m_VBO),
          m_VAO(other.m_VAO), m_EBO(other.m_EBO), tex(other.tex) {
        other.m_VBO = 0;
        other.m_VAO = 0;
        other.m_EBO = 0;
        other.tex = 0;
    }

    Texture2D &operator=(Texture2D &&other) noexcept {
        if (this != &other) {
            m_Unload();

            pos = other.pos;
            size = other.size;
            textureSize = other.textureSize;
            rotAngle = other.rotAngle;
            color = other.color;
            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;
            m_EBO = other.m_EBO;
            tex = other.tex;

            other.m_VBO = 0;
            other.m_VAO = 0;
            other.m_EBO = 0;
            other.tex = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;

  private:
    uint32_t m_VBO{}, m_VAO{}, m_EBO{};

    void m_Load(const std::string &filePath, const TextureFiltering &textureFiltering);
    void m_Unload() const;
};
} // namespace CPL
