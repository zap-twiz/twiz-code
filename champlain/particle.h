#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector3f.h>

#include <iostream>
class Particle {
 public:
  Particle() : mass_(1.0f) {}
  Particle(float mass) : mass_(mass) {}

  virtual ~Particle() {}

  void Integrate(float duration);
  void Integrate2(float duration);
  void ResetAccumulators();

  Vector3f& position() { return position_; }
  Vector3f const & position() const { return position_; }

  Vector3f& velocity() { return velocity_; }
  Vector3f const & velocity() const { return velocity_; }

  Vector3f& acceleration() { return acceleration_; }
  Vector3f const & acceleration() const { return acceleration_; }

  Vector3f& force() { return force_; }
  Vector3f const & force() const { return force_; }

  // if negative, then mass is infinite!
  float& mass() { return mass_; }
  float mass() const { return mass_; }

  bool hasFiniteMass() const { return mass_ >= 0.0f; }

 protected:

  void ApplyForce();

  Vector3f position_;
  Vector3f velocity_;
  Vector3f acceleration_;

  Vector3f force_;

  float mass_;
};

#include <vector>

class ParticleSystem {
 public:
  ParticleSystem() {}
  virtual ~ParticleSystem() {}

  //std::vector<Particle>& particles()
  Particle* NewParticle();

  void TimeStep(float duration);

  std::vector<Particle>& particles() { return particles_; }

 private:
  std::vector<Particle> particles_;
};

class ParticleGenerator {
 public:
};


#endif // PARTICLE_H

