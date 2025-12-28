#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Triangle {
  public:
    glm::vec2 position;
    glm::vec2 size;
    Color color;
    mutable float rotationAngle = 0.0f;

    explicit Triangle(glm::vec2 pos, glm::vec2 size, Color color);
    ~Triangle();

    Triangle(const Triangle &) = delete;
    Triangle &operator=(const Triangle &) = delete;

    Triangle(Triangle &&other) noexcept
        : position(other.position), size(other.size), color(other.color),
          rotationAngle(other.rotationAngle), VBO(other.VBO), VAO(other.VAO) {
        other.VBO = 0;
        other.VAO = 0;
    }

    Triangle &operator=(Triangle &&other) noexcept {
        if (this != &other) {
            if (VAO != 0 && glIsVertexArray(VAO)) {
                glDeleteVertexArrays(1, &VAO);
            }
            if (VBO != 0 && glIsBuffer(VBO)) {
                glDeleteBuffers(1, &VBO);
            }

            position = other.position;
            size = other.size;
            color = other.color;
            rotationAngle = other.rotationAngle;
            VBO = other.VBO;
            VAO = other.VAO;

            other.VBO = 0;
            other.VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, bool filled) const;

  private:
    unsigned int VBO{}, VAO{};
};
} // namespace CPL
