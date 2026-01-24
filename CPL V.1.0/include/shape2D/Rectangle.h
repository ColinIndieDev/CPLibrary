#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Rectangle {
  public:
    glm::vec2 pos;
    glm::vec2 size;
    Color color;
    mutable float rotAngle = 0.0f;

    explicit Rectangle(const glm::vec2 &pos, const glm::vec2 &size, const Color &color);
    ~Rectangle();

    Rectangle(const Rectangle &) = delete;
    Rectangle &operator=(const Rectangle &) = delete;

    Rectangle(Rectangle &&other) noexcept
        : pos(other.pos), size(other.size), color(other.color),
          rotAngle(other.rotAngle), m_OutlineVBO(other.m_OutlineVBO),
          m_OutlineVAO(other.m_OutlineVAO), m_OutlineEBO(other.m_OutlineEBO),
          m_VBO(other.m_VBO), m_VAO(other.m_VAO), m_EBO(other.m_EBO) {
        other.m_OutlineVBO = 0;
        other.m_OutlineVAO = 0;
        other.m_OutlineEBO = 0;
        other.m_VBO = 0;
        other.m_VAO = 0;
        other.m_EBO = 0;
    }

    Rectangle &operator=(Rectangle &&other) noexcept {
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
            if (m_EBO != 0 && glIsBuffer(m_EBO)) {
                glDeleteBuffers(1, &m_EBO);
            }
            if (m_OutlineEBO != 0 && glIsBuffer(m_OutlineEBO)) {
                glDeleteBuffers(1, &m_OutlineEBO);
            }

            pos = other.pos;
            size = other.size;
            color = other.color;
            rotAngle = other.rotAngle;
            m_OutlineVBO = other.m_OutlineVBO;
            m_OutlineVAO = other.m_OutlineVAO;
            m_OutlineEBO = other.m_OutlineEBO;
            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;
            m_EBO = other.m_EBO;

            other.m_OutlineVBO = 0;
            other.m_OutlineVAO = 0;
            other.m_OutlineEBO = 0;
            other.m_VBO = 0;
            other.m_VAO = 0;
            other.m_EBO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, bool filled) const;

  private:
    uint32_t m_OutlineVBO{}, m_OutlineVAO{}, m_OutlineEBO{};
    uint32_t m_VBO{}, m_VAO{}, m_EBO{};
};
} // namespace CPL
