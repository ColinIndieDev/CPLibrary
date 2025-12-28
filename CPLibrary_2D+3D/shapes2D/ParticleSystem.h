#pragma once
#include "../CPL.h"
#include "Texture2D.h"
#include <memory>

namespace CPL {
class Texture2D;
class ParticleSystem {
  public:
    glm::vec2 position;

    ParticleSystem(const glm::vec2 &position) : position(position) {}
    void Update();
    void Draw();
    void AddParticle(Texture2D *texture, const Color &color,
                     const float lifeTime, const glm::vec2 &direction,
                     const glm::vec2 &offset);

    struct Particle {
        glm::vec2 position;
        float curLifeTime = 0;
        float lifeTime;
        glm::vec2 direction;
        bool active = true;
        Texture2D *particleTex;
        Color color;

        Particle(const glm::vec2 &position, Texture2D *texture,
                 const Color &color, const float lifeTime,
                 const glm::vec2 &direction)
            : position(position), lifeTime(lifeTime), direction(direction),
              particleTex(texture), color(color) {}
        void Update() {
            curLifeTime += GetDeltaTime();
            position += direction * glm::vec2(GetDeltaTime());
        }
    };

    std::vector<Particle> particles;
};
} // namespace CPL
