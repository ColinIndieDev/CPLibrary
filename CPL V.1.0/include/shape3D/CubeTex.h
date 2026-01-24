#pragma once

#include "../CPL.h"
#include <glm/glm.hpp>

namespace CPL {
struct Color;
class Shader;
class Texture2D;

class CubeTex {
  public:
    glm::vec3 pos;
    glm::vec3 size;
    Color color;

    explicit CubeTex(const glm::vec3 &pos, const glm::vec3 &size, const Color &color);
    ~CubeTex();

    CubeTex(const CubeTex &) = delete;
    CubeTex &operator=(const CubeTex &) = delete;

    CubeTex(CubeTex &&other) noexcept
        : pos(other.pos), size(other.size),
          color(other.color), m_VBO(other.m_VBO),
          m_VAO(other.m_VAO), m_VBOAtlas(other.m_VBOAtlas), m_VAOAtlas(other.m_VAOAtlas) {
        other.m_VBO = 0;
        other.m_VAO = 0;
        other.m_VBOAtlas = 0;
        other.m_VAOAtlas = 0;
    }

    CubeTex &operator=(CubeTex &&other) noexcept {
        if (this != &other) {
            if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
                glDeleteVertexArrays(1, &m_VAO);
            }
            if (m_VBO != 0 && glIsBuffer(m_VBO)) {
                glDeleteBuffers(1, &m_VBO);
            }
            if (m_VAOAtlas != 0 && glIsVertexArray(m_VAOAtlas)) {
                glDeleteVertexArrays(1, &m_VAOAtlas);
            }
            if (m_VBOAtlas != 0 && glIsBuffer(m_VBOAtlas)) {
                glDeleteBuffers(1, &m_VBOAtlas);
            }

            pos = other.pos;
            size = other.size;
            color = other.color;

            m_VBO = other.m_VBO;
            m_VAO = other.m_VAO;
            m_VBOAtlas = other.m_VBOAtlas;
            m_VAOAtlas = other.m_VAOAtlas;
            other.m_VBO = 0;
            other.m_VAO = 0;
            other.m_VBOAtlas = 0;
            other.m_VAOAtlas = 0;
        }
        return *this;
    }

    void Draw(const Shader &shader, const Texture2D *tex) const;
    void DrawDepth(const Shader &shader, const Texture2D *tex) const;
    void DrawAtlas(const Shader &shader, const Texture2D *atlasTex) const;
    void DrawDepthAtlas(const Shader &shader,
                        const Texture2D *atlasTex) const;

  private:
    void m_InitAtlas();
    uint32_t m_VBO{}, m_VAO{}, m_VBOAtlas{}, m_VAOAtlas{};
};
} // namespace CPL
