#pragma once

#include "../CPL.h"

namespace CPL {
struct Color;

class GlobalLight {
  public:
    explicit GlobalLight(const float intensity, const Color &color)
        : intensity(intensity), color(color) {}

    float intensity = 0;
    Color color{};
};
} // namespace CPL
