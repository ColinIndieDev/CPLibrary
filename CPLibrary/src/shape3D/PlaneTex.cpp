#include "../../include/shape3D/PlaneTex.h"
#include "../../include/CPL.h"
#include "../../include/Shader.h"
#include "../../include/shape2D/Texture2D.h"

namespace CPL {
PlaneTex::PlaneTex(const glm::vec3 &pos, const glm::vec3 &rot,
                   const glm::vec2 &size, const Color &color)
    : pos(pos), rot(rot), size(size), color(color) {
    float sx = size.x / 2.0f;
    float sz = size.y / 2.0f;

    const std::array<float, 48> vertices = {
        -sx, 0.0f, -sz,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         sx, 0.0f, -sz,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         sx, 0.0f,  sz,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         sx, 0.0f,  sz,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -sx, 0.0f,  sz,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -sx, 0.0f, -sz,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
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
}

PlaneTex::~PlaneTex() {
    if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0 && glIsBuffer(m_VBO)) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
}

void PlaneTex::Draw(const Shader &shader, const Texture2D *const tex) const {
    auto transform = glm::mat4(1.0f);
    auto center = pos;
    transform = glm::translate(transform, center);
    transform = glm::rotate(transform, glm::radians(rot.x),
                            glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rot.y),
                            glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rot.z),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, -center);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", pos);
    shader.SetColor("objColor", color);
    shader.SetInt("tex", 0);

    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void PlaneTex::DrawDepth(const Shader &shader,
                         const Texture2D *const tex) const {
    auto model = glm::mat4(1.0f);

    model = glm::translate(model, pos);
    model =
        glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model =
        glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model =
        glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    shader.Use();
    shader.SetMatrix4fv("model", model);
    shader.SetInt("tex", 0);

    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace CPL
