#pragma once

#include "../CPL.h"

namespace CPL {
struct Color;

class PointLight {
  public:
    explicit PointLight(const glm::vec2 &pos, const float radius,
                        const float intensity, const Color &color)
        : pos(pos), radius(radius), intensity(intensity),
          color(color) {}

    glm::vec2 pos{};
    float radius = 0;
    float intensity = 0;
    Color color{};
};
} // namespace CPL
