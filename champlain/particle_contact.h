#ifndef PARTICLE_CONTACT_H
#define PARTICLE_CONTACT_H

#include "vector3f.h"

class Particle;

class ParticleContact {
 public:
  ParticleContact(Particle *actorA,
                  Particle *actorB,
                  float restitution,
                  float depth,
                  Vector3f const & normal);

  Particle* actorA() { return actorA_; }
  Particle* actorB() { return actorB_; }

  float restitution() { return restitution_; }
  float depth() { return depth_; }

  Vector3f const & normal() { return normal_; }

  Vector3f const & actor_A_displacement() const { return actor_a_displacement_; }
  Vector3f const & actor_B_displacement() const { return actor_b_displacement_; }

  void Resolve(float duration);

 protected:

  float SeparatingVelocity() const;

  void ResolveVelocity(float duration);
  void ResolveInterpenetration(float duration);

 private:
  Particle *actorA_;
  Particle *actorB_;

  float restitution_;
  float depth_;

  Vector3f normal_;

  Vector3f actor_a_displacement_;
  Vector3f actor_b_displacement_;
};

class ParticleContactResolver {
 public:
  virtual ~ParticleContactResolver() {}

  virtual void ResolveContacts(ParticleContact* contacts, int num_contacts) = 0;
};

class NaiveParticleContactResolver : public ParticleContactResolver {
 public:
  virtual void ResolveContacts(ParticleContact* contacts, int num_contacts);
};

#endif // PARTICLE_CONTACT_H

