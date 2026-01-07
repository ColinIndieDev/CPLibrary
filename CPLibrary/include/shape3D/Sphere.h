#pragma once
#include "../CPL.h"

namespace CPL {

class Sphere {
  private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

  public:
    glm::vec3 pos;
    float radius;
    Color color;

    explicit Sphere(const glm::vec3 &pos, float radius, const Color &color);
    ~Sphere();

    Sphere(const Sphere &) = delete;
    Sphere &operator=(const Sphere &) = delete;

    Sphere(Sphere &&other) noexcept
        : pos(other.pos), radius(other.radius), color(other.color),
          m_VBO(other.m_VBO), m_VAO(other.m_VAO), m_EBO(other.m_EBO),
          m_Vertices(other.m_Vertices), m_Indices(other.m_Indices) {
        other.m_VBO = 0;
        other.m_VAO = 0;
        other.m_EBO = 0;
    }

    Sphere &operator=(Sphere &&other) noexcept {
        if (this != &other) {
            if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
                glDeleteVertexArrays(1, &m_VAO);
            }
            if (m_VBO != 0 && glIsBuffer(m_VBO)) {
                glDeleteBuffers(1, &m_VBO);
            }
            if (m_EBO != 0 && glIsBuffer(m_EBO)) {
                glDeleteBuffers(1, &m_EBO);
            }

            pos = other.pos;
            radius = other.radius;
            color = other.color;

            m_Vertices = other.m_Vertices;
            m_Indices = other.m_Indices;

            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;
            m_EBO = other.m_EBO;
            other.m_VBO = 0;
            other.m_VAO = 0;
            other.m_EBO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;
    void DrawDepth(const Shader &shader) const;

  private:
    uint32_t m_VBO{}, m_VAO{}, m_EBO{};
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
};
} // namespace CPL
