#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Circle {
  public:
    glm::vec2 pos;
    float radius;
    Color color;

    explicit Circle(const glm::vec2 &pos, float radius, const Color &color);
    ~Circle();

    Circle(const Circle &) = delete;
    Circle &operator=(const Circle &) = delete;

    Circle(Circle &&other) noexcept
        : pos(other.pos), radius(other.radius), color(other.color),
          m_VBO(other.m_VBO), m_VAO(other.m_VAO),
          m_OutlineVBO(other.m_OutlineVBO), m_OutlineVAO(other.m_OutlineVAO),
          m_VertexCount(other.m_VertexCount) {
        other.m_VBO = 0;
        other.m_VAO = 0;
        other.m_OutlineVBO = 0;
        other.m_OutlineVAO = 0;
    }

    Circle &operator=(Circle &&other) noexcept {
        if (this != &other) {
            if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
                glDeleteVertexArrays(1, &m_VAO);
            }
            if (m_VBO != 0 && glIsBuffer(m_VBO)) {
                glDeleteBuffers(1, &m_VBO);
            }
            if (m_OutlineVAO != 0 && glIsVertexArray(m_OutlineVAO)) {
                glDeleteVertexArrays(1, &m_OutlineVAO);
            }
            if (m_OutlineVBO != 0 && glIsBuffer(m_OutlineVBO)) {
                glDeleteBuffers(1, &m_OutlineVBO);
            }

            pos = other.pos;
            radius = other.radius;
            color = other.color;
            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;
            m_OutlineVBO = other.m_OutlineVBO;
            m_OutlineVAO = other.m_OutlineVAO;
            m_VertexCount = other.m_VertexCount;

            other.m_VBO = 0;
            other.m_VAO = 0;
            other.m_OutlineVBO = 0;
            other.m_OutlineVAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;
    void DrawOutline(const Shader &shader) const;

  private:
    uint32_t m_VBO{}, m_VAO{};
    uint32_t m_OutlineVBO{}, m_OutlineVAO{};
    int m_VertexCount = 0;
};
} // namespace CPL
