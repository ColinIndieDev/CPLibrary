#pragma once

#include "../CPL.h"

namespace CPL {
struct Color;

class PointLight3D {
  public:
    explicit PointLight3D(const glm::vec3 &pos, const float intensity,
                          const float constant, const float linear,
                          const float quadratic, const Color &color)
        : pos(pos), intensity(intensity), color(color), constant(constant),
          linear(linear), quadratic(quadratic) {}

    glm::vec3 pos{};
    float intensity = 0;
    Color color{};
    float constant = 0;
    float linear = 0;
    float quadratic = 0;
};
} // namespace CPL
