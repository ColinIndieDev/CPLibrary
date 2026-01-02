#include "ScreenQuad.h"
#include "../Engine.h"
#include "../Shader.h"

namespace CPL {
void ScreenQuad::Init(const int width, const int height) {
    size = glm::vec2(width, height);

    const std::array<float, 30> vertices = {
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f 
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));

    Engine::GetScreenQuadShader().Use();
    Engine::GetScreenQuadShader().SetInt("screenTexture", 0);

    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(size.x), static_cast<int>(size.y), 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           m_TextureColorBuffer, 0);

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<int>(size.x), static_cast<int>(size.y));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, m_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logging::Log(Logging::MessageStates::WARNING, "Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScreenQuad::BeginUseScreen() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
}
void ScreenQuad::EndUseScreen() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ScreenQuad::Draw(const int mode) const {
    Engine::GetScreenQuadShader().Use();
    Engine::GetScreenQuadShader().SetInt("postProcessingMode", mode);

    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void ScreenQuad::DrawCustom(const Shader &shader) const {
    shader.Use();

    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
} // namespace CPL
