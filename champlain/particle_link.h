#ifndef PARTICLE_LINK_H
#define PARTICLE_LINK_H

#include "particle_contact_generator.h"

class Particle;

class ParticleLink : public ParticleContactGenerator {
 public:
  ParticleLink() : particle_a_(0), particle_b_(0) {}
  ParticleLink(Particle *particle_a, Particle *particle_b)
    : particle_a_(particle_a),
      particle_b_(particle_b) {
  }

  Particle* actor_a() const { return particle_a_; }
  void set_actor_a(Particle* particle) { particle_a_ = particle; }

  Particle* actor_b() const { return particle_b_; }
  void set_actor_b(Particle* particle) { particle_b_ = particle; }

  float CurrentLength() const;

  virtual void AddContacts(ParticleSystem& system) = 0;

 protected:
  Particle *particle_a_, *particle_b_;
};

class ParticleRod : public ParticleLink {
 public:
  ParticleRod() : ParticleLink(), length_(0) {}
  ParticleRod(Particle *particle_a, Particle *particle_b, float length)
    : ParticleLink(particle_a, particle_b),
      length_(length) {
  }

  float length() const { return length_; }
  void set_length(float length) { length_ = length; }

  // from ParticleContactGenerator
  virtual void AddContacts(ParticleSystem& system);

 protected:
  float length_;
};

#endif // PARTICLE_LINK_H

