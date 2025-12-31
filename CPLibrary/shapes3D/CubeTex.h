#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;
class Texture2D;

class CubeTex {
  public:
    glm::vec3 position;
    glm::vec3 size;
    Color color;

    explicit CubeTex(glm::vec3 pos, glm::vec3 size, Color color);
    ~CubeTex();

    CubeTex(const CubeTex &) = delete;
    CubeTex &operator=(const CubeTex &) = delete;

    CubeTex(CubeTex &&other) noexcept
        : position(other.position), size(other.size),
          color(other.color), VBO(other.VBO),
          VAO(other.VAO), VBOAtlas(other.VBOAtlas), VAOAtlas(other.VAOAtlas) {
        other.VBO = 0;
        other.VAO = 0;
        other.VBOAtlas = 0;
        other.VAOAtlas = 0;
    }

    CubeTex &operator=(CubeTex &&other) noexcept {
        if (this != &other) {
            if (VAO != 0 && glIsVertexArray(VAO)) {
                glDeleteVertexArrays(1, &VAO);
            }
            if (VBO != 0 && glIsBuffer(VBO)) {
                glDeleteBuffers(1, &VBO);
            }
            if (VAOAtlas != 0 && glIsVertexArray(VAOAtlas)) {
                glDeleteVertexArrays(1, &VAOAtlas);
            }
            if (VBOAtlas != 0 && glIsBuffer(VBOAtlas)) {
                glDeleteBuffers(1, &VBOAtlas);
            }

            position = other.position;
            size = other.size;
            color = other.color;

            VBO = other.VBO;
            VAO = other.VAO;
            VBOAtlas = other.VBOAtlas;
            VAOAtlas = other.VAOAtlas;
            other.VBO = 0;
            other.VAO = 0;
            other.VBOAtlas = 0;
            other.VAOAtlas = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, const Texture2D *texture) const;
    void DrawDepth(const Shader &shader, const Texture2D *texture) const;
    void DrawAtlas(const Shader &shader, const Texture2D *atlasTexture) const;
    void DrawDepthAtlas(const Shader &shader,
                        const Texture2D *atlasTexture) const;

  private:
    void InitAtlas();
    unsigned int VBO{}, VAO{}, VBOAtlas{}, VAOAtlas{};
};
} // namespace CPL
