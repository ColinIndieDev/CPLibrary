#include "Sphere.h"
#include "../Shader.h"
#include <algorithm>

namespace CPL {
Sphere::Sphere(const glm::vec3 pos, const float radius, const Color &color)
    : pos(pos), radius(radius), color(color) {
    const float PI = 3.14159265359f;
    const int res = 250.0f * radius;
    const int stacks = std::clamp(res, 8, 64);
    const int sectors = stacks * 2;

    for (int i = 0; i <= stacks; ++i) {
        float v = (float)i / stacks;
        float theta = v * PI;

        for (int j = 0; j <= sectors; ++j) {
            float u = (float)j / sectors;
            float phi = u * 2.0f * PI;

            glm::vec3 pos;
            pos.x = radius * sin(theta) * cos(phi);
            pos.y = radius * cos(theta);
            pos.z = radius * sin(theta) * sin(phi);

            glm::vec3 normal = glm::normalize(pos);

            m_Vertices.push_back({pos, normal, glm::vec2(u, 1.0f - v)});
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;

            m_Indices.push_back(first);
            m_Indices.push_back(second);
            m_Indices.push_back(first + 1);

            m_Indices.push_back(second);
            m_Indices.push_back(second + 1);
            m_Indices.push_back(first + 1);
        }
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex),
                 m_Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_Indices.size() * sizeof(unsigned int), m_Indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, uv));
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
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Sphere::DrawDepth(const Shader &shader) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    shader.Use();
    shader.SetMatrix4fv("model", model);
    
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
} // namespace CPL
