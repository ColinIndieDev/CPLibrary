#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;
class Texture2D;

class PlaneTex {
  public:
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec2 size;
    Color color;

    explicit PlaneTex(const glm::vec3 &pos, const glm::vec3 &rot,
                      const glm::vec2 &size, const Color &color);
    ~PlaneTex();

    PlaneTex(const PlaneTex &) = delete;
    PlaneTex &operator=(const PlaneTex &) = delete;

    PlaneTex(PlaneTex &&other) noexcept
        : pos(other.pos), rot(other.rot), size(other.size), color(other.color),
          m_VBO(other.m_VBO), m_VAO(other.m_VAO) {
        other.m_VBO = 0;
        other.m_VAO = 0;
    }

    PlaneTex &operator=(PlaneTex &&other) noexcept {
        if (this != &other) {
            if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
                glDeleteVertexArrays(1, &m_VAO);
            }
            if (m_VBO != 0 && glIsBuffer(m_VBO)) {
                glDeleteBuffers(1, &m_VBO);
            }

            pos = other.pos;
            rot = other.rot;
            size = other.size;
            color = other.color;

            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;
            other.m_VBO = 0;
            other.m_VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, const Texture2D *tex) const;
    void DrawDepth(const Shader &shader, const Texture2D *tex) const;

  private:
    uint32_t m_VBO{}, m_VAO{};
};
} // namespace CPL
