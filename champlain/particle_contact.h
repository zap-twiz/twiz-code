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

  Particle* actorA() const { return actorA_; }
  Particle* actorB() const { return actorB_; }

  float restitution() const { return restitution_; }
  float depth() const { return depth_; }
  float set_depth(float depth) { depth_ = depth; }

  Vector3f const & normal() const { return normal_; }

  Vector3f const & actor_A_displacement() const { return actor_a_displacement_; }
  Vector3f const & actor_B_displacement() const { return actor_b_displacement_; }

  void Resolve(float duration);
  float SeparatingVelocity() const;

 protected:
  void ResolveVelocity(float duration);
  void ResolveInterpenetration(float duration);

 private:
  Particle *actorA_;
  Particle *actorB_;

  float restitution_;
  float depth_;

  Vector3f normal_;

  // Track the amount of displacement applied to the particles by this contact
  // If the contact is resolved multiple times, this may be necessary.
  Vector3f actor_a_displacement_;
  Vector3f actor_b_displacement_;
};

class ParticleContactResolver {
 public:
  virtual ~ParticleContactResolver() {}

  virtual void ResolveContacts(ParticleContact* contacts, int num_contacts,
                               float duration) = 0;

  virtual void SetPasses(int passes) = 0;

};

class NaiveParticleContactResolver : public ParticleContactResolver {
 public:
  virtual void ResolveContacts(ParticleContact* contacts, int num_contacts,
                               float duration);
  virtual void SetPasses(int passes) {}
};

class MultiPassParticleContactResolver : public ParticleContactResolver {
 public:
  MultiPassParticleContactResolver() : passes_(0) {}
  virtual void ResolveContacts(ParticleContact* contacts, int num_contacts,
                               float duration);

  virtual void SetPasses(int passes) { passes_ = passes; }

 private:
  int passes_;
};

#endif // PARTICLE_CONTACT_H

