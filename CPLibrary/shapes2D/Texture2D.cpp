#include "Texture2D.h"
#include "../Shader.h"
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace CPL {
Texture2D::Texture2D(const std::string &filePath, const glm::vec2 &pos,
                     const glm::vec2 &size, const Color &color,
                     const TextureFiltering &textureFiltering)
    : pos(pos), size(size), textureSize(0), color(color) {
    const std::array<float, 20> vertices = {
        size.x, 0.0f,   0.0f,  1.0f, 1.0f,
        size.x, size.y, 0.0f,  1.0f, 0.0f,

        0.0f,   size.y, 0.0f,  0.0f, 0.0f,
        0.0f,   0.0f,   0.0f,  0.0f, 1.0f
    };
    constexpr std::array<uint32_t, 6> indices = {
        0, 1, 3, 
        1, 2, 3
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    textureFiltering == TextureFiltering::LINEAR ? GL_LINEAR
                                                                 : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    textureFiltering == TextureFiltering::LINEAR ? GL_LINEAR
                                                                 : GL_NEAREST);
    stbi_set_flip_vertically_on_load(1);
    int width = 0;
    int height = 0;
    unsigned char *data =
        stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    GLenum format = 0;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    if (static_cast<bool>(data)) {
        this->textureSize.x = static_cast<float>(width);
        this->textureSize.y = static_cast<float>(height);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format),
                     static_cast<GLsizei>(this->textureSize.x),
                     static_cast<GLsizei>(this->textureSize.y), 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Logging::Log(Logging::MessageStates::ERROR, "Failed to load texture");
    }
    stbi_image_free(data);
}
Texture2D::Texture2D(const std::string &filePath, const glm::vec2 &size,
                     const TextureFiltering &textureFiltering)
    : pos(0.0f), size(size), textureSize(0), color(WHITE) {
    const std::array<float, 20> vertices = {
        size.x, 0.0f,   0.0f,  1.0f, 1.0f,
        size.x, size.y, 0.0f,  1.0f, 0.0f,

        0.0f,   size.y, 0.0f,  0.0f, 0.0f,
        0.0f,   0.0f,   0.0f,  0.0f, 1.0f
    };
    constexpr std::array<uint32_t, 6> indices = {
        0, 1, 3, 
        1, 2, 3
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    textureFiltering == TextureFiltering::LINEAR ? GL_LINEAR
                                                                 : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    textureFiltering == TextureFiltering::LINEAR ? GL_LINEAR
                                                                 : GL_NEAREST);
    stbi_set_flip_vertically_on_load(1);
    int width = 0;
    int height = 0;
    unsigned char *data =
        stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (static_cast<bool>(data)) {
        this->textureSize.x = static_cast<float>(width);
        this->textureSize.y = static_cast<float>(height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     static_cast<GLsizei>(this->textureSize.x),
                     static_cast<GLsizei>(this->textureSize.y), 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Logging::Log(Logging::MessageStates::ERROR, "Failed to load texture");
    }
    stbi_image_free(data);
}

void Texture2D::Unload() const {
    if (tex != 0)
        glDeleteTextures(1, &tex);
    if (m_VAO != 0)
        glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0)
        glDeleteBuffers(1, &m_VBO);
}

void Texture2D::Draw(const Shader &shader) const {
    auto transform = glm::mat4(1.0f);
    const glm::vec2 center = {pos.x + (size.x / 2), pos.y + (size.y / 2)};
    transform = glm::translate(transform, glm::vec3(center, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotAngle),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(-center, 0.0f));

    shader.SetMatrix4fv("transform", transform);
    shader.SetVector3f("offset", glm::vec3(pos, 0.0f));
    shader.SetColor("inputColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace CPL
