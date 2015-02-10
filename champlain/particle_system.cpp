#include "particle_system.h"

#include "particle_contact_generator.h"
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
  UpdateForces(duration);
  Integrate(duration);
  ProcessCollisions(duration);
}

void ParticleSystem::Integrate(float duration) {
  auto iter = particles_.begin();
  auto end = particles_.end();
  for (; iter != end; ++iter) {
    iter->Integrate(duration);
  }
}

void ParticleSystem::UpdateForces(float duration) {
  // Reset all particule accumulators
  auto iter(particles_.begin()), end(particles_.end());
  for (; iter != end; ++iter) {
    iter->ResetAccumulators();
  }

  auto regIter(forceMapping_.begin()), regEnd(forceMapping_.end());
  for (; regIter != regEnd; ++regIter) {
    regIter->generator->applyForce(regIter->particle, duration);
  }
}

void ParticleSystem::ProcessCollisions(float duration) {

  // Note that this may free memory ...
  contacts_.resize(0, ParticleContact(NULL, NULL, 0, 0, Vector3f(0, 0, 0)));

  auto iter = contact_generators_.begin();
  auto end = contact_generators_.end();
  for (; iter != end; ++iter) {
    (*iter)->AddContacts(*this);
  }

  // Heuristic - set the maximum number of passes to 10x the number of collisions!
  resolver_->SetPasses(contacts_.size()*10);
  resolver_->ResolveContacts(&contacts_[0], contacts_.size(), duration);
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
