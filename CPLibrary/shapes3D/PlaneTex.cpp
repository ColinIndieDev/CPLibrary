#include "PlaneTex.h"
#include "../CPL.h"
#include "../Shader.h"
#include "../shapes2D/Texture2D.h"

namespace CPL {
PlaneTex::PlaneTex(const glm::vec3 pos, const glm::vec3 rotation,
                   const glm::vec2 size, const Color color)
    : position(pos), rotation(rotation), size(size), color(color) {
    float sx = size.x / 2.0f;
    float sz = size.y / 2.0f;

    const float vertices[] = {
        // positions        // normals          // texcoords
        -sx,  0.0f, -sz,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, sx,   0.0f, -sz,  0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, sx,   0.0f, sz,   0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

        sx,   0.0f, sz,   0.0f, 1.0f, 0.0f, 1.0f, 1.0f, -sx,  0.0f, sz,   0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, -sx,  0.0f, -sz,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

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

PlaneTex::~PlaneTex() {
    if (VAO != 0 && glIsVertexArray(VAO)) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0 && glIsBuffer(VBO)) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
}

void PlaneTex::Draw(const Shader &shader, const Texture2D *texture) const {
    auto transform = glm::mat4(1.0f);
    auto center = position;
    transform = glm::translate(transform, center);
    transform = glm::rotate(transform, glm::radians(rotation.x),
                            glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y),
                            glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, -center);

    glm::mat4 view = GetCam3D().GetViewMatrix();
    glm::mat4 projection =
        GetCam3D().GetProjectionMatrix(GetScreenWidth() / GetScreenHeight());
    shader.SetMatrix4fv("projection", projection * view);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(position));
    shader.SetColor("inputColor", color);
    shader.SetInt("ourTexture", 0);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void PlaneTex::DrawDepth(const Shader &shader, const Texture2D *texture) const {
    auto model = glm::mat4(1.0f);

    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    shader.Use();
    shader.SetMatrix4fv("model", model);
    shader.SetInt("ourTexture", 0);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace CPL
