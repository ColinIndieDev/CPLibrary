#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Triangle {
  public:
    glm::vec2 pos;
    glm::vec2 size;
    Color color;
    mutable float rotAngle = 0.0f;

    explicit Triangle(const glm::vec2 &pos, const glm::vec2 &size,
                      const Color &color);
    ~Triangle();

    Triangle(const Triangle &) = delete;
    Triangle &operator=(const Triangle &) = delete;

    Triangle(Triangle &&other) noexcept
        : pos(other.pos), size(other.size), color(other.color),
          rotAngle(other.rotAngle), m_VBO(other.m_VBO), m_VAO(other.m_VAO) {
        other.m_VBO = 0;
        other.m_VAO = 0;
    }

    Triangle &operator=(Triangle &&other) noexcept {
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
            rotAngle = other.rotAngle;
            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;

            other.m_VBO = 0;
            other.m_VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, bool filled) const;

  private:
    uint32_t m_VBO{}, m_VAO{};
};
} // namespace CPL
