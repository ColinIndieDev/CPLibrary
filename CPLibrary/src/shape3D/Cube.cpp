#include "../../include/CPL.h"
#include "../../include/shape2D/Texture2D.h"
#include "../../include/shape3D/Cube.h"
#include "../../include/Shader.h"

namespace CPL {
Cube::Cube(const glm::vec3 &pos, const glm::vec3 &size, const Color &color)
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
        -sx, -sy, sz,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         sx, -sy, sz,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         sx,  sy, sz,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         sx,  sy, sz,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -sx,  sy, sz,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -sx, -sy, sz,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

        // Left face (X–)
        -sx,  sy, sz,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -sx,  sy, -sz, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -sx, -sy, -sz, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -sx, -sy, -sz, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -sx, -sy, sz,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -sx,  sy, sz,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

        // Right face (X+)
         sx,  sy, sz,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         sx, -sy, -sz,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         sx,  sy, -sz,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         sx, -sy, -sz,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         sx,  sy, sz,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         sx, -sy, sz,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

        // Bottom face (Y–)
        -sx, -sy, -sz, 0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
         sx, -sy, -sz, 0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
         sx, -sy, sz,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         sx, -sy, sz,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        -sx, -sy, sz,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -sx, -sy, -sz, 0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

        // Top face (Y+)
        -sx,  sy, -sz, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         sx,  sy, sz,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         sx,  sy, -sz, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         sx,  sy, sz,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -sx,  sy, -sz, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -sx,  sy, sz,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
    };


    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

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
Cube::~Cube() {
    if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0 && glIsBuffer(m_VBO)) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
}

void Cube::Draw(const Shader &shader) const {
    auto transform = glm::mat4(1.0f);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", pos);
    shader.SetColor("objColor", color);

    shader.SetInt("tex", 0);
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Engine::GetWhiteTex()->tex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Cube::DrawDepth(const Shader &shader) const {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    shader.Use();
    shader.SetMatrix4fv("model", model);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
} // namespace CPL
