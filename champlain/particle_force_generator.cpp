
#include "particle_force_generator.h"
#include "particle.h"

Vector3f const ParticleGravityForce::kGravityVector(0.0f, -1.0f, 0.0f);
void ParticleGravityForce::applyForce(Particle* particle, float duration) {
  if (!particle->hasFiniteMass()) {
    return;
  }

  particle->force() += particle->mass() * constant_ * kGravityVector;
}

void ParticleDampingForce::applyForce(Particle* particle, float duration) {
  if (!particle->hasFiniteMass()) {
    return;
  }

  Vector3f direction = particle->velocity();
  float speed = direction.length();
  direction.normalize();
  particle->force() += -1.0 * direction * (speed * k1_ + speed * speed * k2_);
}

void ParticleAnchoredSpringForce::applyForce(Particle* particle, float duration) {
  if (!particle->hasFiniteMass()) {
    return;
  }

  Vector3f spring(particle->position() - anchorPoint_);
  float stretch = spring.length();
  Vector3f direction = spring.normalize();

  particle->force() += -1.0f * direction * constant_ * (stretch - restLength_);
}
