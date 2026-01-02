#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Line {
  public:
    glm::vec2 startPos;
    glm::vec2 endPos;
    Color color;

    explicit Line(const glm::vec2 &startPos, const glm::vec2 &endPos, const Color &color);
    ~Line();

    Line(const Line &) = delete;
    Line &operator=(const Line &) = delete;

    Line(Line &&other) noexcept
        : startPos(other.startPos), endPos(other.endPos), color(other.color),
          m_VBO(other.m_VBO), m_VAO(other.m_VAO) {
        other.m_VBO = 0;
        other.m_VAO = 0;
    }

    Line &operator=(Line &&other) noexcept {
        if (this != &other) {
            if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
                glDeleteVertexArrays(1, &m_VAO);
            }
            if (m_VBO != 0 && glIsBuffer(m_VBO)) {
                glDeleteBuffers(1, &m_VBO);
            }

            startPos = other.startPos;
            endPos = other.endPos;
            color = other.color;
            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;

            other.m_VBO = 0;
            other.m_VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;

  private:
    uint32_t m_VBO{}, m_VAO{};
};
} // namespace CPL
