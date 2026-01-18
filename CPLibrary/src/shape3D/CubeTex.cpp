#include "../../include/shape3D/CubeTex.h"
#include "../../include/CPL.h"
#include "../../include/Shader.h"
#include "../../include/shape2D/Texture2D.h"
#include "../../include/shape3D/Cube.h"

namespace CPL {
CubeTex::CubeTex(const glm::vec3 &pos, const glm::vec3 &size,
                 const Color &color)
    : pos(pos), size(size), color(color) {
    float sx = size.x / 2.0f;
    float sy = size.y / 2.0f;
    float sz = size.z / 2.0f;

    const std::array<float, 288> vertices = {
        // Back face (Z-)
        -sx, -sy, -sz,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
         sx,  sy, -sz,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
         sx, -sy, -sz,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
         sx,  sy, -sz,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
        -sx, -sy, -sz,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -sx,  sy, -sz,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,

        // Front face (Z+)
        -sx, -sy, sz,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         sx, -sy, sz,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         sx,  sy, sz,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         sx,  sy, sz,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -sx,  sy, sz,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -sx, -sy, sz,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

        // Left face (X–)
        -sx,  sy,  sz,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -sx,  sy, -sz,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -sx, -sy, -sz,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -sx, -sy, -sz,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -sx, -sy,  sz,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -sx,  sy,  sz,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

        // Right face (X+)
         sx,  sy,  sz,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         sx, -sy, -sz,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         sx,  sy, -sz,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         sx, -sy, -sz,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         sx,  sy,  sz,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         sx, -sy,  sz,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

        // Bottom face (Y–)
        -sx, -sy, -sz,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
         sx, -sy, -sz,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
         sx, -sy,  sz,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         sx, -sy,  sz,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        -sx, -sy,  sz,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -sx, -sy, -sz,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

        // Top face (Y+)
        -sx, sy, -sz,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         sx, sy,  sz,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         sx, sy, -sz,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         sx, sy,  sz,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -sx, sy, -sz,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -sx, sy,  sz,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_InitAtlas();
}

void CubeTex::m_InitAtlas() {
    float sx = size.x / 2.0f;
    float sy = size.y / 2.0f;
    float sz = size.z / 2.0f;

    float w = 1.0f / 3.0f;
    float h = 1.0f / 2.0f;

    const std::array<float, 288> atlasVertices = {
        // Back face (Z-)
        -sx, -sy, -sz,  0.0f, 0.0f, -1.0f,  2*w, 2*h,
         sx,  sy, -sz,  0.0f, 0.0f, -1.0f,  3*w, 1*h,
         sx, -sy, -sz,  0.0f, 0.0f, -1.0f,  3*w, 2*h,
         sx,  sy, -sz,  0.0f, 0.0f, -1.0f,  3*w, 1*h,
        -sx, -sy, -sz,  0.0f, 0.0f, -1.0f,  2*w, 2*h,
        -sx,  sy, -sz,  0.0f, 0.0f, -1.0f,  2*w, 1*h,

        // Front face (Z+)
        -sx, -sy, sz,  0.0f, 0.0f, 1.0f,  1*w, 1*h,
         sx, -sy, sz,  0.0f, 0.0f, 1.0f,  2*w, 1*h,
         sx,  sy, sz,  0.0f, 0.0f, 1.0f,  2*w, 0*h,
         sx,  sy, sz,  0.0f, 0.0f, 1.0f,  2*w, 0*h,
        -sx,  sy, sz,  0.0f, 0.0f, 1.0f,  1*w, 0*h,
        -sx, -sy, sz,  0.0f, 0.0f, 1.0f,  1*w, 1*h,

        // Left face (X-)
        -sx,  sy,  sz,  -1.0f, 0.0f, 0.0f,  3*w, 0*h,
        -sx,  sy, -sz,  -1.0f, 0.0f, 0.0f,  2*w, 0*h,
        -sx, -sy, -sz,  -1.0f, 0.0f, 0.0f,  2*w, 1*h,
        -sx, -sy, -sz,  -1.0f, 0.0f, 0.0f,  2*w, 1*h,
        -sx, -sy,  sz,  -1.0f, 0.0f, 0.0f,  3*w, 1*h,
        -sx,  sy,  sz,  -1.0f, 0.0f, 0.0f,  3*w, 0*h,

        // Right face (X+)
        sx,  sy,  sz,  1.0f, 0.0f, 0.0f,  1*w, 0*h,
        sx, -sy, -sz,  1.0f, 0.0f, 0.0f,  0*w, 1*h,
        sx,  sy, -sz,  1.0f, 0.0f, 0.0f,  0*w, 0*h,
        sx, -sy, -sz,  1.0f, 0.0f, 0.0f,  0*w, 1*h,
        sx,  sy,  sz,  1.0f, 0.0f, 0.0f,  1*w, 0*h,
        sx, -sy,  sz,  1.0f, 0.0f, 0.0f,  1*w, 1*h,

        // Bottom face (Y-)
        -sx, -sy, -sz,  0.0f, -1.0f, 0.0f,  0*w, 1*h,
         sx, -sy, -sz,  0.0f, -1.0f, 0.0f,  1*w, 1*h,
         sx, -sy,  sz,  0.0f, -1.0f, 0.0f,  1*w, 2*h,
         sx, -sy,  sz,  0.0f, -1.0f, 0.0f,  1*w, 2*h,
        -sx, -sy,  sz,  0.0f, -1.0f, 0.0f,  0*w, 2*h,
        -sx, -sy, -sz,  0.0f, -1.0f, 0.0f,  0*w, 1*h,

        // Top face (Y+)
        -sx, sy, -sz,  0.0f, 1.0f, 0.0f,  1*w, 1*h,
         sx, sy,  sz,  0.0f, 1.0f, 0.0f,  2*w, 2*h,
         sx, sy, -sz,  0.0f, 1.0f, 0.0f,  2*w, 1*h,
         sx, sy,  sz,  0.0f, 1.0f, 0.0f,  2*w, 2*h,
        -sx, sy, -sz,  0.0f, 1.0f, 0.0f,  1*w, 1*h,
        -sx, sy,  sz,  0.0f, 1.0f, 0.0f,  1*w, 2*h
    };

    glGenVertexArrays(1, &m_VAOAtlas);
    glGenBuffers(1, &m_VBOAtlas);
    glBindVertexArray(m_VAOAtlas);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOAtlas);
    glBufferData(GL_ARRAY_BUFFER, sizeof(atlasVertices), atlasVertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

CubeTex::~CubeTex() {
    if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0 && glIsBuffer(m_VBO)) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_VAOAtlas != 0 && glIsVertexArray(m_VAOAtlas)) {
        glDeleteVertexArrays(1, &m_VAOAtlas);
        m_VAOAtlas = 0;
    }
    if (m_VBOAtlas != 0 && glIsBuffer(m_VBOAtlas)) {
        glDeleteBuffers(1, &m_VBOAtlas);
        m_VBOAtlas = 0;
    }
}

void CubeTex::Draw(const Shader &shader, const Texture2D *const tex) const {
    auto transform = glm::mat4(1.0f);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", pos);
    shader.SetColor("objColor", color);
    shader.SetInt("tex", 0);
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->tex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void CubeTex::DrawDepth(const Shader &shader,
                        const Texture2D *const tex) const {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    shader.Use();
    shader.SetMatrix4fv("model", model);
    shader.SetInt("tex", 0);

    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->tex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void CubeTex::DrawAtlas(const Shader &shader,
                        const Texture2D *const atlasTex) const {
    auto transform = glm::mat4(1.0f);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", pos);
    shader.SetColor("objColor", color);
    shader.SetInt("tex", 0);

    glBindVertexArray(m_VAOAtlas);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTex->tex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void CubeTex::DrawDepthAtlas(const Shader &shader,
                             const Texture2D *const atlasTex) const {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    shader.Use();
    shader.SetMatrix4fv("model", model);
    shader.SetInt("tex", 0);

    glBindVertexArray(m_VAOAtlas);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTex->tex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace CPL
