#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "particle.h"
#include "particle_contact.h"

#include <vector>

class ParticleForceGenerator;
class ParticleContactGenerator;

class ParticleSystem {
 public:
  ParticleSystem() {
    // TODO - Convert to an id-based system!!
    particles_.reserve(10000);
  }
  virtual ~ParticleSystem();

  Particle* NewParticle();

  void TimeStep(float duration);

  std::vector<Particle>& particles() { return particles_; }
  std::vector<Particle> const & particles() const { return particles_; }

  ParticleForceGenerator* AddForceGenerator(ParticleForceGenerator* generator);
  void LinkForceGenerator(Particle* particle, ParticleForceGenerator* generator);

  ParticleContactGenerator* AddContactGenerator(ParticleContactGenerator* generator);

  void AddContact(ParticleContact const & contact);

 protected:
  struct ForceRegistration {
    Particle* particle;
    ParticleForceGenerator* generator;
  };

  std::vector<ForceRegistration> forceMapping_;
  std::vector<ParticleForceGenerator*> forceGenerators_;
  std::vector<ParticleContactGenerator*> contact_generators_;

  std::vector<ParticleContact> contacts_;
  std::vector<Particle> particles_;
};

#endif // PARTICLESYSTEM_H