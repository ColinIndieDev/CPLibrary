#pragma once

#include "../CPL.h"

namespace CPL {
    struct Plane {
        glm::vec3 normal;
        float distance;
        
        Plane() : normal(0, 0, 0), distance(0) {}
        
        float GetDistance(const glm::vec3& point) const {
            return glm::dot(normal, point) + distance;
        }
    };
    
    class Frustum {
    public:
        enum Side { LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR };
        Plane planes[6];
        
        void Update(const glm::mat4& viewProjection) {
            glm::mat4 vp = glm::transpose(viewProjection);
            
            // Left plane
            planes[LEFT].normal = glm::vec3(vp[3] + vp[0]);
            planes[LEFT].distance = vp[3][3] + vp[0][3];
            
            // Right plane
            planes[RIGHT].normal = glm::vec3(vp[3] - vp[0]);
            planes[RIGHT].distance = vp[3][3] - vp[0][3];
            
            // Bottom plane
            planes[BOTTOM].normal = glm::vec3(vp[3] + vp[1]);
            planes[BOTTOM].distance = vp[3][3] + vp[1][3];
            
            // Top plane
            planes[TOP].normal = glm::vec3(vp[3] - vp[1]);
            planes[TOP].distance = vp[3][3] - vp[1][3];
            
            // Near plane
            planes[NEAR].normal = glm::vec3(vp[3] + vp[2]);
            planes[NEAR].distance = vp[3][3] + vp[2][3];
            
            // Far plane
            planes[FAR].normal = glm::vec3(vp[3] - vp[2]);
            planes[FAR].distance = vp[3][3] - vp[2][3];
            
            // Normalisieren
            for (auto& plane : planes) {
                float length = glm::length(plane.normal);
                plane.normal /= length;
                plane.distance /= length;
            }
        }
        
        // AABB (Axis-Aligned Bounding Box) Test
        bool IsCubeVisible(const glm::vec3& center, const glm::vec3& halfSize) const {
            for (const auto& plane : planes) {
                float radius = 
                    halfSize.x * std::abs(plane.normal.x) +
                    halfSize.y * std::abs(plane.normal.y) +
                    halfSize.z * std::abs(plane.normal.z);
                
                float distance = plane.GetDistance(center);
                
                if (distance < -radius) {
                    return false;
                }
            }
            return true;
        }
        
        /* Sphere Test (einfacher, aber ungenauer)
        bool IsSphereVisible(const glm::vec3& center, float radius) const {
            for (const auto& plane : planes) {
                if (plane.GetDistance(center) < -radius) {
                    return false;
                }
            }
            return true;
        }
        */
    };
}
