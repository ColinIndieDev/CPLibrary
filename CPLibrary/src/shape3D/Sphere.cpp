#include "../../include/shape3D/Sphere.h"
#include "../../include/Shader.h"
#include <algorithm>

namespace CPL {
Sphere::Sphere(const glm::vec3 &pos, const float radius, const Color &color)
    : pos(pos), radius(radius), color(color) {
    const int res = static_cast<int>(250.0f * radius);
    const int stacks = std::clamp(res, 8, 64);
    const int sectors = stacks * 2;

    for (int i = 0; i <= stacks; ++i) {
        float v = static_cast<float>(i) / static_cast<float>(stacks);
        float theta = v * static_cast<float>(std::numbers::pi);

        for (int j = 0; j <= sectors; ++j) {
            float u = static_cast<float>(j) / static_cast<float>(sectors);
            float phi = u * 2.0f * static_cast<float>(std::numbers::pi);

            glm::vec3 pos;
            pos.x = radius * std::sin(theta) * std::cos(phi);
            pos.y = radius * std::cos(theta);
            pos.z = radius * std::sin(theta) * std::sin(phi);

            glm::vec3 normal = glm::normalize(pos);

            m_Vertices.push_back({pos, normal, glm::vec2(u, 1.0f - v)});
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = (i * (sectors + 1)) + j;
            int second = first + sectors + 1;

            m_Indices.push_back(first);
            m_Indices.push_back(first + 1);
            m_Indices.push_back(second);

            m_Indices.push_back(first + 1);
            m_Indices.push_back(second + 1);
            m_Indices.push_back(second);
        }
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<int>(m_Vertices.size() * sizeof(Vertex)),
                 m_Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<int>(m_Indices.size() * sizeof(uint32_t)),
                 m_Indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Sphere::~Sphere() {
    if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0 && glIsBuffer(m_VBO)) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_EBO != 0 && glIsBuffer(m_EBO)) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
}

void Sphere::Draw(const Shader &shader) const {
    auto transform = glm::mat4(1.0f);

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(pos));
    shader.SetColor("inputColor", color);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(m_Indices.size()),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
void Sphere::DrawDepth(const Shader &shader) const {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    shader.Use();
    shader.SetMatrix4fv("model", model);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
} // namespace CPL
