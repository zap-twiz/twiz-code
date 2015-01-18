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

class ParticleAnchoredSpringForce : public ParticleForceGenerator {
 public:
  ParticleAnchoredSpringForce(Vector3f const & point, float restLength, float constant)
    : anchorPoint_(point), restLength_(restLength), constant_(constant) {}

  virtual void applyForce(Particle* particle, float duration);

 private:
  Vector3f anchorPoint_;
  float restLength_;
  float constant_;
};

class ParticleDampingForce : public ParticleForceGenerator {
 public:
  ParticleDampingForce(float k1, float k2) : k1_(k1), k2_(k2) {}
  virtual void applyForce(Particle* particle, float duration);

 private:
  float k1_;
  float k2_;
};

// TODO buoyancy, etc ....

#endif // PARTICLE_FORCE_GENERATOR_H

