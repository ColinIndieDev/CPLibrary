#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Cube {
  public:
    glm::vec3 pos;
    glm::vec3 size;
    Color color;

    explicit Cube(const glm::vec3 &pos, const glm::vec3 &size,
                  const Color &color);
    ~Cube();

    Cube(const Cube &) = delete;
    Cube &operator=(const Cube &) = delete;

    Cube(Cube &&other) noexcept
        : pos(other.pos), size(other.size), color(other.color),
          m_VBO(other.m_VBO), m_VAO(other.m_VAO) {
        other.m_VBO = 0;
        other.m_VAO = 0;
    }

    Cube &operator=(Cube &&other) noexcept {
        if (this != &other) {
            if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
                glDeleteVertexArrays(1, &m_VAO);
            }
            if (m_VBO != 0 && glIsBuffer(m_VBO)) {
                glDeleteBuffers(1, &m_VBO);
            }

            pos = other.pos;
            size = other.size;
            color = other.color;

            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;
            other.m_VBO = 0;
            other.m_VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;
    void DrawDepth(const Shader &shader) const;

  private:
    uint32_t m_VBO{}, m_VAO{};
};
} // namespace CPL
