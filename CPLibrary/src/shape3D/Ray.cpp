#include "../../include/shape2D/Texture2D.h"
#include "../../include/shape3D/Ray.h"
#include "../../include/Shader.h"

namespace CPL {
Ray::Ray(const glm::vec3 &startPos, const glm::vec3 &endPos,
           const Color &color)
    : startPos(startPos), endPos(endPos), color(color) {
    const std::array<float, 16> vertices = {
        startPos.x, startPos.y, startPos.z, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
        endPos.x, endPos.y, endPos.z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
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
Ray::~Ray() {
    if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0 && glIsBuffer(m_VBO)) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
}

void Ray::Draw(const Shader &shader) const {
    shader.SetMatrix4fv("transform", glm::mat4(1.0f));
    shader.SetVector3f("offset", glm::vec3(0.0f));
    shader.SetColor("objColor", color);

    shader.SetInt("tex", 0);
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Engine::GetWhiteTex()->tex);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace CPL
