#include "../CPL.h"
#include "Cube.h"
#include "../Shader.h"

namespace CPL {
Cube::Cube(const glm::vec3 pos, const glm::vec3 size, const Color color)
    : position(pos), size(size), color(color) {
    float sx = size.x / 2.0f;
    float sy = size.y / 2.0f;
    float sz = size.z / 2.0f;

    const float vertices[] = {
        // Back face (Z-)
        -sx, -sy, -sz,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        sx,  sy, -sz,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        sx, -sy, -sz,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        sx,  sy, -sz,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -sx, -sy, -sz,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -sx,  sy, -sz,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

        // Front face (Z+)
        -sx, -sy, sz,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        sx, -sy, sz,   0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        sx,  sy, sz,   0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        sx,  sy, sz,   0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -sx,  sy, sz,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -sx, -sy, sz,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        // Left face (X–)
        -sx,  sy, sz,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -sx,  sy, -sz, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -sx, -sy, -sz, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -sx, -sy, -sz, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -sx, -sy, sz,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -sx,  sy, sz,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        // Right face (X+)
        sx,  sy, sz,   1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        sx, -sy, -sz,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        sx,  sy, -sz,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        sx, -sy, -sz,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        sx,  sy, sz,   1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        sx, -sy, sz,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Bottom face (Y–)
        -sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        sx, -sy, sz,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        sx, -sy, sz,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -sx, -sy, sz,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -sx, -sy, -sz, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        // Top face (Y+)
        -sx,  sy, -sz, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        sx,  sy, sz,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        sx,  sy, -sz, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        sx,  sy, sz,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -sx,  sy, -sz, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -sx,  sy, sz,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
Cube::~Cube() {
    if (VAO != 0 && glIsVertexArray(VAO)) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0 && glIsBuffer(VBO)) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
}

void Cube::Draw(const Shader &shader) const {
    auto transform = glm::mat4(1.0f);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(position));
    shader.SetColor("inputColor", color);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void Cube::DrawDepth(const Shader &shader) const {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader.Use();
    shader.SetMatrix4fv("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
} // namespace CPL
