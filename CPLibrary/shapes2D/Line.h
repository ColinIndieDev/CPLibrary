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

    explicit Line(glm::vec2 startPos, glm::vec2 endPos, const Color &color);
    ~Line();

    Line(const Line &) = delete;
    Line &operator=(const Line &) = delete;

    Line(Line &&other) noexcept
        : startPos(other.startPos), endPos(other.endPos), color(other.color),
          VBO(other.VBO), VAO(other.VAO) {
        other.VBO = 0;
        other.VAO = 0;
    }

    Line &operator=(Line &&other) noexcept {
        if (this != &other) {
            if (VAO != 0 && glIsVertexArray(VAO)) {
                glDeleteVertexArrays(1, &VAO);
            }
            if (VBO != 0 && glIsBuffer(VBO)) {
                glDeleteBuffers(1, &VBO);
            }

            startPos = other.startPos;
            endPos = other.endPos;
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
