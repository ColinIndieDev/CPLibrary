#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Circle {
  public:
    glm::vec2 position;
    float radius;
    Color color;
    mutable float rotationAngle = 0.0f;

    explicit Circle(glm::vec2 pos, float radius, Color color);
    ~Circle();

    Circle(const Circle &) = delete;
    Circle &operator=(const Circle &) = delete;

    Circle(Circle &&other) noexcept
        : position(other.position), radius(other.radius), color(other.color),
          rotationAngle(other.rotationAngle), VBO(other.VBO), VAO(other.VAO),
          outlineVBO(other.outlineVBO), outlineVAO(other.outlineVAO),
          vertexCount(other.vertexCount) {
        other.VBO = 0;
        other.VAO = 0;
        other.outlineVBO = 0;
        other.outlineVAO = 0;
    }

    Circle &operator=(Circle &&other) noexcept {
        if (this != &other) {
            if (VAO != 0 && glIsVertexArray(VAO)) {
                glDeleteVertexArrays(1, &VAO);
            }
            if (VBO != 0 && glIsBuffer(VBO)) {
                glDeleteBuffers(1, &VBO);
            }
            if (outlineVAO != 0 && glIsVertexArray(outlineVAO)) {
                glDeleteVertexArrays(1, &outlineVAO);
            }
            if (outlineVBO != 0 && glIsBuffer(outlineVBO)) {
                glDeleteBuffers(1, &outlineVBO);
            }

            position = other.position;
            radius = other.radius;
            color = other.color;
            rotationAngle = other.rotationAngle;
            VBO = other.VBO;
            VAO = other.VAO;
            outlineVBO = other.outlineVBO;
            outlineVAO = other.outlineVAO;
            vertexCount = other.vertexCount;

            other.VBO = 0;
            other.VAO = 0;
            other.outlineVBO = 0;
            other.outlineVAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;
    void DrawOutline(const Shader &shader) const;

  private:
    unsigned int VBO{}, VAO{};
    unsigned int outlineVBO{}, outlineVAO{};
    int vertexCount = 0;
};
} // namespace CPL
