
#include "particle_force_generator.h"
#include "particle.h"

Vector3f const ParticleGravityForce::kGravityVector(0.0f, -1.0f, 0.0f);
void ParticleGravityForce::applyForce(Particle* particle, float duration) {

  if (!particle->hasFiniteMass()) {
    return;
  }

  particle->force() += particle->mass() * constant_ * kGravityVector;
}
