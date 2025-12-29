#include "ParticleSystem.h"

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
        DrawTex2D(p.particleTex, p.position, p.color);
    }
}

void ParticleSystem::AddParticle(Texture2D *texture, const Color &color,
                                 const float lifeTime,
                                 const glm::vec2 &direction,
                                 const glm::vec2 &offset) {
    particles.emplace_back(
        Particle(position + offset, texture, color, lifeTime, direction));
}
} // namespace CPL
