#pragma once

#include "../CPL.h"
#include <algorithm>

namespace CPL {
struct Plane {
    glm::vec3 normal;
    float dist = 0;

    Plane() : normal(0) {}

    [[nodiscard]] float GetDistance(const glm::vec3 &point) const {
        return glm::dot(normal, point) + dist;
    }
};

class Frustum {
  public:
    enum class Side : uint8_t { LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR };
    std::array<Plane, 6> planes;

    void Update(const glm::mat4 &viewProjection) {
        glm::mat4 vp = glm::transpose(viewProjection);

        planes[static_cast<int>(Side::LEFT)].normal = glm::vec3(vp[3] + vp[0]);
        planes[static_cast<int>(Side::LEFT)].dist = vp[3][3] + vp[0][3];

        planes[static_cast<int>(Side::RIGHT)].normal = glm::vec3(vp[3] - vp[0]);
        planes[static_cast<int>(Side::RIGHT)].dist = vp[3][3] - vp[0][3];

        planes[static_cast<int>(Side::BOTTOM)].normal =
            glm::vec3(vp[3] + vp[1]);
        planes[static_cast<int>(Side::BOTTOM)].dist = vp[3][3] + vp[1][3];

        planes[static_cast<int>(Side::TOP)].normal = glm::vec3(vp[3] - vp[1]);
        planes[static_cast<int>(Side::TOP)].dist = vp[3][3] - vp[1][3];

        planes[static_cast<int>(Side::NEAR)].normal = glm::vec3(vp[3] + vp[2]);
        planes[static_cast<int>(Side::NEAR)].dist = vp[3][3] + vp[2][3];

        planes[static_cast<int>(Side::FAR)].normal = glm::vec3(vp[3] - vp[2]);
        planes[static_cast<int>(Side::FAR)].dist = vp[3][3] - vp[2][3];

        for (auto &plane : planes) {
            float length = glm::length(plane.normal);
            plane.normal /= length;
            plane.dist /= length;
        }
    }

    [[nodiscard]] bool IsCubeVisible(const glm::vec3 &center,
                                     const glm::vec3 &halfSize) const {
        return std::ranges::all_of(planes, [&](const Plane &plane) {
            float radius = (halfSize.x * std::abs(plane.normal.x)) +
                           (halfSize.y * std::abs(plane.normal.y)) +
                           (halfSize.z * std::abs(plane.normal.z));
            float distance = plane.GetDistance(center);

            return distance >= -radius;
        });
    }

    [[nodiscard]] bool IsSphereVisible(const glm::vec3 &center,
                                       const float radius) const {
        return std::ranges::all_of(planes, [&](const Plane &plane) {
            return plane.GetDistance(center) >= -radius;
        });
    }
};
} // namespace CPL
