#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;

class Rectangle {
  public:
    glm::vec2 position;
    glm::vec2 size;
    Color color;
    mutable float rotationAngle = 0.0f;

    explicit Rectangle(glm::vec2 pos, glm::vec2 size, Color color);
    ~Rectangle();

    Rectangle(const Rectangle &) = delete;
    Rectangle &operator=(const Rectangle &) = delete;

    Rectangle(Rectangle &&other) noexcept
        : position(other.position), size(other.size), color(other.color),
          rotationAngle(other.rotationAngle), outlineVBO(other.outlineVBO),
          outlineVAO(other.outlineVAO), outlineEBO(other.outlineEBO),
          VBO(other.VBO), VAO(other.VAO), EBO(other.EBO) {
        other.outlineVBO = 0;
        other.outlineVAO = 0;
        other.outlineEBO = 0;
        other.VBO = 0;
        other.VAO = 0;
        other.EBO = 0;
    }

    Rectangle &operator=(Rectangle &&other) noexcept {
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
            if (EBO != 0 && glIsBuffer(EBO)) {
                glDeleteBuffers(1, &EBO);
            }
            if (outlineEBO != 0 && glIsBuffer(outlineEBO)) {
                glDeleteBuffers(1, &outlineEBO);
            }

            position = other.position;
            size = other.size;
            color = other.color;
            rotationAngle = other.rotationAngle;
            outlineVBO = other.outlineVBO;
            outlineVAO = other.outlineVAO;
            outlineEBO = other.outlineEBO;
            VBO = other.VBO;
            VAO = other.VAO;
            EBO = other.EBO;

            other.outlineVBO = 0;
            other.outlineVAO = 0;
            other.outlineEBO = 0;
            other.VBO = 0;
            other.VAO = 0;
            other.EBO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, bool filled) const;
    void DrawLight(const Shader &shader, bool filled) const;

  private:
    unsigned int outlineVBO{}, outlineVAO{}, outlineEBO{};
    unsigned int VBO{}, VAO{}, EBO{};
};
} // namespace CPL
