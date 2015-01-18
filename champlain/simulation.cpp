
#include "simulation.h"

#include "particle_force_generator.h"

#include "random.h"

Simulation::~Simulation() {
}

#include <iostream>

void Simulation::Step(float duration) {

  // Step the owned simulation
  ParticleSystem &system = getSystem();
  system.TimeStep(duration);
  //getSystem().TimeStep(duration);
}

void FireworksSimulation::Reset() {
  Random generator;

  ParticleSystem &system = getSystem();


  ParticleGravityForce *gravity = new ParticleGravityForce(20);
  ParticleDampingForce *damping = new ParticleDampingForce(0.05, 0.05);
  ParticleAnchoredSpringForce *spring = new ParticleAnchoredSpringForce(
        Vector3f(0, 0, 0), 10, 50);
  system.AddForceGenerator(gravity);
  system.AddForceGenerator(damping);
  system.AddForceGenerator(spring);

  for (int x = 0; x < 5; ++x) {
    // Create 100 particles, with random positions and velocities
    Particle* particle = system.NewParticle();

    system.LinkForceGenerator(particle, gravity);
    system.LinkForceGenerator(particle, damping);
    system.LinkForceGenerator(particle, spring);

    particle->position() = Vector3f(0, 0, 0);
    particle->velocity() = generator.Rand() * 500.0f * generator.RandHemisphere();
    particle->acceleration() = Vector3f(0.0f, 0.0f, 0.0f);
  }
}

void FireworksSimulation::Pause() {
  // TODO
}

void FireworksSimulation::Step(float duration) {
  // TODO: Refactor
  Simulation::Step(duration);
}

