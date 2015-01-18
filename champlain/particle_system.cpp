#include "particle_system.h"

#include "particle_force_generator.h"

ParticleSystem::~ParticleSystem() {
  for (ParticleForceGenerator* generator : forceGenerators_) {
    delete generator;
  }

  for (ParticleContactGenerator* generator : contact_generators_) {
    delete generator;
  }
}

Particle* ParticleSystem::NewParticle() {
  particles_.push_back(Particle());
  return &particles_[particles_.size() - 1];
}

void ParticleSystem::TimeStep(float duration) {

  // Reset all particule accumulators
  auto iter(particles_.begin()), end(particles_.end());
  for (; iter != end; ++iter) {
    iter->ResetAccumulators();
  }

  auto regIter(forceMapping_.begin()), regEnd(forceMapping_.end());
  for (; regIter != regEnd; ++regIter) {
    regIter->generator->applyForce(regIter->particle, duration);
  }

  iter = particles_.begin();
  end = particles_.end();
  for (; iter != end; ++iter) {
    iter->Integrate(duration);
  }
}

ParticleForceGenerator* ParticleSystem::AddForceGenerator(ParticleForceGenerator *generator) {
  forceGenerators_.push_back(generator);
  return generator;
}

void ParticleSystem::LinkForceGenerator(Particle* particle, ParticleForceGenerator* generator) {
  ForceRegistration registration = {particle, generator};
  forceMapping_.push_back(registration);
}

ParticleContactGenerator* ParticleSystem::AddContactGenerator(ParticleContactGenerator *generator) {
  contact_generators_.push_back(generator);
}

void ParticleSystem::AddContact(ParticleContact const &contact) {
  contacts_.push_back(contact);
}
