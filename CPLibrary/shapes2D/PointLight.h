#pragma once

#include "../CPL.h"

namespace CPL {
struct Color;

class PointLight {
  public:
    explicit PointLight(const glm::vec2 position, const float radius,
                        const float intensity, const Color &color)
        : position(position), radius(radius), intensity(intensity),
          color(color) {}

    glm::vec2 position{};
    float radius = 0;
    float intensity = 0;
    Color color{};
};
} // namespace CPL
