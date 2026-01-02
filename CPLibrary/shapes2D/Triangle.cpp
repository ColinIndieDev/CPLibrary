#include "../CPL.h"
#include "Triangle.h"
#include "../Shader.h"

namespace CPL {
    Triangle::Triangle(const glm::vec2 &pos, const glm::vec2 &size, const Color &color) : pos(pos), size(size), color(color) {
        const std::array<float, 9> vertices = {
            0.0f,       0.0f,   0.0f, 
            size.x,     0.0f,   0.0f, 
            size.x / 2, size.y, 0.0f 
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    Triangle::~Triangle() {
        if (m_VAO != 0 && glIsVertexArray(m_VAO)) {
            glDeleteVertexArrays(1, &m_VAO);
            m_VAO = 0;
        }
        if (m_VBO != 0 && glIsBuffer(m_VBO)) {
            glDeleteBuffers(1, &m_VBO);
            m_VBO = 0;
        }
    }

    void Triangle::Draw(const Shader& shader, const bool filled) const {
        auto transform = glm::mat4(1.0f);
        const glm::vec2 center = {pos.x + (size.x / 2), pos.y + (size.y / 2)};
        transform = glm::translate(transform, glm::vec3(center, 0.0f));
        transform = glm::rotate(transform, -glm::radians(rotAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::translate(transform, glm::vec3(-center, 0.0f));

        shader.SetMatrix4fv("transform", transform);
        shader.SetVector3f("offset", glm::vec3(pos, 0));
        shader.SetColor("inputColor", color);
        glBindVertexArray(m_VAO);
        if (filled) glDrawArrays(GL_TRIANGLES, 0, 3);
        else glDrawArrays(GL_LINE_LOOP, 0, 3);
        glBindVertexArray(0);
    }
}
