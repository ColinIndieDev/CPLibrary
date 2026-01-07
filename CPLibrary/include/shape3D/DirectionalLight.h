#pragma once

#include "../CPL.h"

namespace CPL {
class DirectionalLight {
  public:
    explicit DirectionalLight(const glm::vec3 &dir, const glm::vec3 &ambient,
                              const glm::vec3 &diffuse,
                              const glm::vec3 &specular)
        : dir(dir), ambient(ambient), diffuse(diffuse), specular(specular) {}

    glm::vec3 dir{};
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
};
} // namespace CPL
