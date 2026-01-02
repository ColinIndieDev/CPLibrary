#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace CPL {
class ShadowMap {
  public:
    uint32_t depthMapFBO{};
    uint32_t depthMap{};
    uint32_t shadowWidth, shadowHeight;

    ShadowMap(uint32_t res);
    ~ShadowMap();

    ShadowMap(const ShadowMap &) = delete;
    ShadowMap &operator=(const ShadowMap &) = delete;

    ShadowMap(ShadowMap &&other) noexcept
        : depthMapFBO(other.depthMapFBO), depthMap(other.depthMap),
          shadowWidth(other.shadowWidth), shadowHeight(other.shadowHeight) {
        other.depthMapFBO = 0;
        other.depthMap = 0;
    }

    ShadowMap &operator=(ShadowMap &&other) noexcept {
        if (this != &other) {
            if (depthMapFBO != 0 && glIsFramebuffer(depthMapFBO)) {
                glDeleteFramebuffers(1, &depthMapFBO);
            }
            if (depthMap != 0 && glIsTexture(depthMap)) {
                glDeleteTextures(1, &depthMap);
            }

            shadowWidth = other.shadowWidth;
            shadowHeight = other.shadowHeight;
            depthMapFBO = other.depthMapFBO;
            depthMap = other.depthMap;

            other.depthMapFBO = 0;
            other.depthMap = 0;
        }
        return *this;
    }

    void BeginDepthPass(const glm::mat4 &lightSpaceMatrix) const;
    static void EndDepthPass();
    void BindForReading(uint32_t textureUnit = 1) const;
};
} // namespace CPL
