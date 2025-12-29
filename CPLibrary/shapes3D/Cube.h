#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Cube {
  public:
    glm::vec3 position;
    glm::vec3 size;
    Color color;

    explicit Cube(glm::vec3 pos, glm::vec3 size, Color color);
    ~Cube();

    Cube(const Cube &) = delete;
    Cube &operator=(const Cube &) = delete;

    Cube(Cube &&other) noexcept
        : position(other.position), size(other.size), color(other.color),
          VBO(other.VBO), VAO(other.VAO) {
        other.VBO = 0;
        other.VAO = 0;
    }

    Cube &operator=(Cube &&other) noexcept {
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

            VBO = other.VBO;
            VAO = other.VAO;
            other.VBO = 0;
            other.VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader) const;

  private:
    unsigned int VBO{}, VAO{};
};
} // namespace CPL
