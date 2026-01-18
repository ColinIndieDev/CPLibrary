#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Ray {
  public:
    glm::vec3 startPos;
    glm::vec3 endPos;
    Color color;

    explicit Ray(const glm::vec3 &startPos, const glm::vec3 &endPos,
                 const Color &color);
    ~Ray();

    Ray(const Ray &) = delete;
    Ray &operator=(const Ray &) = delete;

    Ray(Ray &&other) noexcept
        : startPos(other.startPos), endPos(other.endPos), color(other.color),
          m_VBO(other.m_VBO), m_VAO(other.m_VAO) {
        other.m_VBO = 0;
        other.m_VAO = 0;
    }

    Ray &operator=(Ray &&other) noexcept {
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
