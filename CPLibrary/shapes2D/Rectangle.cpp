#include "Rectangle.h"
#include "../CPL.h"
#include "../Shader.h"

namespace CPL {
Rectangle::Rectangle(const glm::vec2 &pos, const glm::vec2 &size,
                     const Color &color)
    : pos(pos), size(size), color(color) {
    const std::array<float, 12> vertices = {
        size.x, 0.0f,   0.0f, 
        size.x, size.y, 0.0f, 
        0.0f,   size.y, 0.0f,   
        0.0f,   0.0f,   0.0f
    };
    constexpr std::array<uint32_t, 6> indices = {
        0, 1, 3, 
        1, 2, 3
    };
    constexpr std::array<uint32_t, 4> outlineIndices = {
        0, 1, 2, 3
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &m_OutlineVAO);
    glGenBuffers(1, &m_OutlineVBO);
    glGenBuffers(1, &m_OutlineEBO);
    glBindVertexArray(m_OutlineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_OutlineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_OutlineEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(outlineIndices),
                 outlineIndices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
Rectangle::~Rectangle() {
    if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0 && glIsBuffer(m_VBO)) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_OutlineVAO != 0 && glIsVertexArray(m_OutlineVAO)) {
        glDeleteVertexArrays(1, &m_OutlineVAO);
        m_OutlineVAO = 0;
    }
    if (m_OutlineVBO != 0 && glIsBuffer(m_OutlineVBO)) {
        glDeleteBuffers(1, &m_OutlineVBO);
        m_OutlineVBO = 0;
    }
    if (m_EBO != 0 && glIsBuffer(m_EBO)) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    if (m_OutlineEBO != 0 && glIsBuffer(m_OutlineEBO)) {
        glDeleteBuffers(1, &m_OutlineEBO);
        m_OutlineEBO = 0;
    }
}

void Rectangle::Draw(const Shader &shader, const bool filled) const {
    auto transform = glm::mat4(1.0f);
    const glm::vec2 center = {pos.x + (size.x / 2), pos.y + (size.y / 2)};
    transform = glm::translate(transform, glm::vec3(center, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotAngle),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(-center, 0.0f));

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(pos, 0.0f));
    shader.SetColor("inputColor", color);
    if (filled) {
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    } else {
        glBindVertexArray(m_OutlineVAO);
        glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}
} // namespace CPL
