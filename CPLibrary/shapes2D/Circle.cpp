#include "Circle.h"
#include "../CPL.h"
#include "../Shader.h"
#include <cmath>
#include <vector>

#define PI 3.14159265358979323846
namespace CPL {
Circle::Circle(const glm::vec2 pos, const float radius, const Color color)
    : position(pos), radius(radius), color(color) {
    std::vector<float> vertices;
    const int segments = std::ceil(radius);
    vertices.reserve(segments * 3);
    for (int i = 0; i <= segments; i++) {
        const float theta = 2 * static_cast<float>(PI) /
                            static_cast<float>(segments) *
                            static_cast<float>(i);
        float x = 0.0f + radius * std::cos(theta);
        float y = 0.0f + radius * std::sin(theta);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0);
    }

    glGenVertexArrays(1, &m_OutlineVAO);
    glGenBuffers(1, &m_OutlineVBO);
    glBindVertexArray(m_OutlineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_OutlineVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    for (int i = 0; i < 3; i++) {
        vertices.push_back(0);
    }
    m_VertexCount = static_cast<int>(vertices.size()) / 3;
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
Circle::~Circle() {
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
}

void Circle::Draw(const Shader &shader) const {
    auto transform = glm::mat4(1.0f);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(position, 0.0f));
    shader.SetColor("inputColor", color);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_VertexCount);
    glBindVertexArray(0);
}

void Circle::DrawOutline(const Shader &shader) const {
    auto transform = glm::mat4(1.0f);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(position, 0.0f));
    shader.SetColor("inputColor", color);

    glBindVertexArray(m_OutlineVAO);
    glDrawArrays(GL_LINE_LOOP, 0, m_VertexCount - 1);
    glBindVertexArray(0);
}
} // namespace CPL
