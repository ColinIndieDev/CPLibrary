#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace CPL {
class ShadowMap {
  public:
    ShadowMap(uint32_t res);
    ~ShadowMap();

    ShadowMap(const ShadowMap &) = delete;
    ShadowMap &operator=(const ShadowMap &) = delete;

    ShadowMap(ShadowMap &&other) noexcept
        :m_DepthMapFBO(other.m_DepthMapFBO), m_DepthMap(other.m_DepthMap),
          m_ShadowWidth(other.m_ShadowWidth), m_ShadowHeight(other.m_ShadowHeight) {
        other.m_DepthMapFBO = 0;
        other.m_DepthMap = 0;
    }

    ShadowMap &operator=(ShadowMap &&other) noexcept {
        if (this != &other) {
            if (m_DepthMapFBO != 0 && glIsFramebuffer(m_DepthMapFBO)) {
                glDeleteFramebuffers(1, &m_DepthMapFBO);
            }
            if (m_DepthMap != 0 && glIsTexture(m_DepthMap)) {
                glDeleteTextures(1, &m_DepthMap);
            }

            m_ShadowWidth = other.m_ShadowWidth;
            m_ShadowHeight = other.m_ShadowHeight;
            m_DepthMapFBO = other.m_DepthMapFBO;
            m_DepthMap = other.m_DepthMap;

            other.m_DepthMapFBO = 0;
            other.m_DepthMap = 0;
        }
        return *this;
    }

    void BeginDepthPass(const glm::mat4 &lightSpaceMatrix) const;
    static void EndDepthPass();
    void BindForReading(uint32_t textureUnit = 1) const;

  private:
    uint32_t m_DepthMapFBO{};
    uint32_t m_DepthMap{};
    uint32_t m_ShadowWidth, m_ShadowHeight;
};
} // namespace CPL
