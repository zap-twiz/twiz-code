
#include "particle.h"

void Particle::Integrate2(float duration) {
  ApplyForce();

  position_ += velocity_*duration + 0.5f * acceleration_*duration*duration;
  velocity_ += acceleration_*duration;
}

void Particle::Integrate(float duration) {
  ApplyForce();

  position_ += velocity_ * duration;
  velocity_ += acceleration_ * duration;
}

void Particle::ResetAccumulators() {
  acceleration_ = Vector3f(0.0f, 0.0f, 0.0f);
  force_ = Vector3f(0.0f, 0.0f, 0.0f);
}

void Particle::ApplyForce() {
  if (!hasFiniteMass()) {
    return;
  }
  acceleration_ = force_ * (1.0f / mass_);
}



