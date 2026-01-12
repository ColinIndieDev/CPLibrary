#include "../../include/shape3D/ShadowMap.h"
#include "../../include/Engine.h"

namespace CPL {
ShadowMap::ShadowMap(const uint32_t res) : m_ShadowWidth(res), m_ShadowHeight(res) {
    glGenFramebuffers(1, &m_DepthMapFBO);

    glGenTextures(1, &m_DepthMap);
    glBindTexture(GL_TEXTURE_2D, m_DepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<int>(res),
                 static_cast<int>(res), 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    std::array<float, 4> borderColor = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
                     borderColor.data());

    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           m_DepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap() {
    if (m_DepthMapFBO != 0 && glIsFramebuffer(m_DepthMapFBO)) {
        glDeleteFramebuffers(1, &m_DepthMapFBO);
    }
    if (m_DepthMap != 0 && glIsTexture(m_DepthMap)) {
        glDeleteTextures(1, &m_DepthMap);
    }
}

void ShadowMap::BeginDepthPass(const glm::mat4 &lightSpaceMatrix) const {
    glViewport(0, 0, static_cast<int>(m_ShadowWidth),
               static_cast<int>(m_ShadowHeight));
    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
}

void ShadowMap::EndDepthPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, static_cast<int>(Engine::GetScreenWidth()),
               static_cast<int>(Engine::GetScreenHeight()));
}

void ShadowMap::BindForReading(const uint32_t textureUnit) const {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_DepthMap);
}
} // namespace CPL
