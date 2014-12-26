
#include "simulation.h"

#include "particle_force_generator.h"

#include "random.h"

Simulation::~Simulation() {
  auto iter(forceGenerators_.begin()), end(forceGenerators_.end());
  for (; iter != end; ++iter) {
    delete *iter;
  }
#if 0
  for (ParticleForceGenerator* generator : forceGenerators_) {
    delete generator;
  }
#endif
}

#include <iostream>

void Simulation::Step(float duration) {
  // Reset all particule accumulators

#if 1
  ParticleSystem& system = getSystem();
  std::vector<Particle>& particles = system.particles();

  auto iter(particles.begin()), end(particles.end());
  for (; iter != end; ++iter) {
    iter->ResetAccumulators();
  }

#if 1
  auto regIter(forceMapping_.begin()), regEnd(forceMapping_.end());
  for (; regIter != regEnd; ++regIter) {
    std::cout << regIter->generator << std::endl;
    std::cout << regIter->particle << std::endl;

    regIter->generator->applyForce(regIter->particle, duration);
  }
#endif

#if 0
  for (ForceRegistration const & forceRegistration : forceMapping_) {
    //std::cout << forceRegistration.particle << std::endl;
    //std::cout << forceRegistration.generator << std::endl;
    forceRegistration.generator->applyForce(forceRegistration.particle, duration);
  }
#endif
#endif

  getSystem().TimeStep(duration);
}

ParticleForceGenerator* Simulation::AddForceGenerator(ParticleForceGenerator *generator) {
  forceGenerators_.push_back(generator);
  return generator;
}

void Simulation::LinkForceGenerator(Particle* particle, ParticleForceGenerator* generator) {
  ForceRegistration registration = {particle, generator};
  forceMapping_.push_back(registration);
}

void FireworksSimulation::Reset() {
  Random generator;

  ParticleGravityForce *gravity = new ParticleGravityForce();
  AddForceGenerator(gravity);

  for (int x = 0; x < 1000; ++x) {
    // Create 100 particles, with random positions and velocities
    Particle* particle = system_.NewParticle();

    LinkForceGenerator(particle, gravity);

    particle->position() = Vector3f(0, 0, 0);
    particle->velocity() = generator.Rand() * 5.0f * generator.RandHemisphere();
    //particle.velocity() = particle.acceleration() = Vector3f(0, 0, 0);
    particle->acceleration() = Vector3f(0.0f, 0.0f, 0.0f);
    //particle.acceleration() = Vector3f(0, -1.0f, 0.0f);
  }
}

void FireworksSimulation::Pause() {
  // TODO
}

void FireworksSimulation::Step(float duration) {
  // TODO: Refactor
  Simulation::Step(duration);
}

