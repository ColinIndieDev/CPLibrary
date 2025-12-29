#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace CPL {
class ShadowMap {
  public:
    unsigned int depthMapFBO;
    unsigned int depthMap;
    unsigned int shadowWidth, shadowHeight;

    ShadowMap(const unsigned int resolution);
    ~ShadowMap();

    void BeginDepthPass(const glm::mat4 &lightSpaceMatrix);
    void EndDepthPass();
    void BindForReading(unsigned int textureUnit = 1);
};
} // namespace CPL
