#pragma once

#include "../CPL.h"

namespace CPL {
class DirectionalLight {
  public:
    DirectionalLight(const glm::vec3 &dir, const Color &ambient,
                              const glm::vec3 &diffuse,
                              const glm::vec3 &specular)
        : dir(dir), ambient(ambient), diffuse(diffuse), specular(specular) {}

    glm::vec3 dir{};
    Color ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
};
} // namespace CPL
