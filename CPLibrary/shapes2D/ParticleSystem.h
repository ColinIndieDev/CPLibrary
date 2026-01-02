#pragma once
#include "../CPL.h"
#include "Texture2D.h"
#include <memory>

namespace CPL {
class Texture2D;
class ParticleSystem {
  public:
    glm::vec2 pos;

    ParticleSystem(const glm::vec2 &pos) : pos(pos) {}
    void Update();
    void Draw();
    void AddParticle(Texture2D *tex, const Color &color, float lifeTime,
                     const glm::vec2 &dir, const glm::vec2 &offset);

    struct Particle {
        glm::vec2 pos;
        float curLifeTime = 0;
        float lifeTime;
        glm::vec2 dir;
        bool active = true;
        Texture2D *particleTex;
        Color color;

        Particle(const glm::vec2 &pos, Texture2D *const tex, const Color &color,
                 const float lifeTime, const glm::vec2 &dir)
            : pos(pos), lifeTime(lifeTime), dir(dir), particleTex(tex),
              color(color) {}
        void Update() {
            curLifeTime += GetDeltaTime();
            pos += dir * glm::vec2(GetDeltaTime());
        }
    };

    std::vector<Particle> particles;
};
} // namespace CPL
