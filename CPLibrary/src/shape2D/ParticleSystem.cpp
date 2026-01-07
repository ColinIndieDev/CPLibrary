#include "../../include/shape2D/ParticleSystem.h"

namespace CPL {
void ParticleSystem::Update() {
    for (auto &p : particles) {
        p.Update();
        if (p.curLifeTime > p.lifeTime) {
            p.active = false;
        }
    }
    std::erase_if(particles, [](const Particle &p) { return !p.active; });
}

void ParticleSystem::Draw() {
    for (auto &p : particles) {
        DrawTex2D(p.particleTex, p.pos, p.color);
    }
}

void ParticleSystem::AddParticle(Texture2D *const tex, const Color &color,
                                 const float lifeTime,
                                 const glm::vec2 &dir,
                                 const glm::vec2 &offset) {
    particles.emplace_back(
        pos + offset, tex, color, lifeTime, dir);
}
} // namespace CPL
