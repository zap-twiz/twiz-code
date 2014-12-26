#ifndef PARTICLE_FORCE_GENERATOR_H
#define PARTICLE_FORCE_GENERATOR_H

#include "vector3f.h"

class Particle;

class ParticleForceGenerator {
 public:
  virtual ~ParticleForceGenerator() {}

  virtual void applyForce(Particle* particle, float duration) = 0;
};

class ParticleGravityForce : public ParticleForceGenerator {
 public:
  static const float kEarth = 9.8f;
  static const Vector3f kGravityVector;

  ParticleGravityForce(float constant = kEarth) : constant_(constant) {}

  virtual void applyForce(Particle* particle, float duration);

 private:
  float constant_;
};

class Spring {
 public:

};

class ParticleSpringForce : public ParticleForceGenerator {
 public:
};

class ParticleDampingForce : public ParticleForceGenerator {
 public:
};

// TODO buoyancy, etc ....

#endif // PARTICLE_FORCE_GENERATOR_H

