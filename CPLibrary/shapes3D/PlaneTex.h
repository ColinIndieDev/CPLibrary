#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;
class Texture2D;

class PlaneTex {
  public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec2 size;
    Color color;

    explicit PlaneTex(glm::vec3 pos, glm::vec3 rotation, glm::vec2 size,
                      Color color);
    ~PlaneTex();

    PlaneTex(const PlaneTex &) = delete;
    PlaneTex &operator=(const PlaneTex &) = delete;

    PlaneTex(PlaneTex &&other) noexcept
        : position(other.position), rotation(other.rotation), size(other.size),
          color(other.color), VBO(other.VBO), VAO(other.VAO) {
        other.VBO = 0;
        other.VAO = 0;
    }

    PlaneTex &operator=(PlaneTex &&other) noexcept {
        if (this != &other) {
            if (VAO != 0 && glIsVertexArray(VAO)) {
                glDeleteVertexArrays(1, &VAO);
            }
            if (VBO != 0 && glIsBuffer(VBO)) {
                glDeleteBuffers(1, &VBO);
            }

            position = other.position;
            rotation = other.rotation;
            size = other.size;
            color = other.color;

            VBO = other.VBO;
            VAO = other.VAO;
            other.VBO = 0;
            other.VAO = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, const Texture2D *texture) const;
    void DrawDepth(const Shader &shader, const Texture2D *texture) const;

  private:
    unsigned int VBO{}, VAO{};
};
} // namespace CPL
