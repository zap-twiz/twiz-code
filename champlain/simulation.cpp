
#include "simulation.h"
#include "particle_contact.h"
#include "particle_contact_generator.h"
#include "particle_force_generator.h"
#include "particle_link.h"

#include "random.h"

Simulation::~Simulation() {
}

void Simulation::Step(float duration) {

  // Step the owned simulation
  ParticleSystem &system = getSystem();
  system.TimeStep(duration);
  //getSystem().TimeStep(duration);
}

BaseSimulation::BaseSimulation() {
  //resolver_ = new NaiveParticleContactResolver();
  resolver_ = new MultiPassParticleContactResolver();

  ParticleSystem &system = getSystem();
  system.SetParticleContactResolver(resolver_);
}

BaseSimulation::~BaseSimulation() {
  delete resolver_;
}

void BaseSimulation::Pause() {
  // TODO
}

void FireworksSimulation::Reset() {
  Random generator;

  ParticleSystem &system = getSystem();

  // Note - these are leaking!
  ParticleGravityForce *gravity = new ParticleGravityForce(20);
  ParticleDampingForce *damping = new ParticleDampingForce(0.05, 0.05);
  ParticleAnchoredSpringForce *spring = new ParticleAnchoredSpringForce(
        Vector3f(0, 0, 0), 10, 50);
  system.AddForceGenerator(gravity);
  system.AddForceGenerator(damping);
  system.AddForceGenerator(spring);

  ParticleGroundContactGenerator *ground = new ParticleGroundContactGenerator();
  system.AddContactGenerator(ground);

  for (int x = 0; x < 900; ++x) {
    // Create 100 particles, with random positions and velocities
    Particle* particle = system.NewParticle();

    system.LinkForceGenerator(particle, gravity);
    system.LinkForceGenerator(particle, damping);
    system.LinkForceGenerator(particle, spring);

    particle->position() = Vector3f(0, 0, 0);
    particle->velocity() = generator.Rand() * 500.0f * generator.RandHemisphere();
    particle->velocity().x() * 100;
    particle->acceleration() = Vector3f(0.0f, 0.0f, 0.0f);
  }
}

void SimpleGravitySimulation::Reset() {
  ParticleSystem &system = getSystem();

  // Note - these are leaking!
  ParticleGravityForce *gravity = new ParticleGravityForce(20);
  ParticleDampingForce *damping = new ParticleDampingForce(0.05, 0.05);
  system.AddForceGenerator(gravity);
  system.AddForceGenerator(damping);

  ParticleGroundContactGenerator *ground = new ParticleGroundContactGenerator();
  system.AddContactGenerator(ground);

  Particle *particle = system.NewParticle();

  system.LinkForceGenerator(particle, gravity);
  system.LinkForceGenerator(particle, damping);

  particle->position() = Vector3f(0.0f, 0.0f, 0.0f);
  particle->mass() = 100.0f;
  particle->velocity() = Vector3f(1.0f, 0.0f, 0.0f);
}

void MultiRodSimulation::Reset() {
  Random generator;

  ParticleSystem &system = getSystem();

  // Note - these are leaking!
  ParticleGravityForce *gravity = new ParticleGravityForce(9.8f);
  ParticleDampingForce *damping = new ParticleDampingForce(0.05, 0.05);
  system.AddForceGenerator(gravity);
  system.AddForceGenerator(damping);

  ParticleGroundContactGenerator *ground = new ParticleGroundContactGenerator();
  system.AddContactGenerator(ground);

  Particle *particle_a = system.NewParticle();
  Particle *particle_b = system.NewParticle();
  Particle *particle_c = system.NewParticle();
  Particle *particle_d = system.NewParticle();

  system.LinkForceGenerator(particle_a, gravity);
  system.LinkForceGenerator(particle_b, gravity);
  system.LinkForceGenerator(particle_c, gravity);
  system.LinkForceGenerator(particle_d, gravity);

  system.LinkForceGenerator(particle_a, damping);
  system.LinkForceGenerator(particle_b, damping);
  system.LinkForceGenerator(particle_c, damping);
  system.LinkForceGenerator(particle_d, damping);

  particle_a->position() = Vector3f(0.0f, 4.0f, 0.0f);
  particle_a->velocity() = Vector3f(0.0f, -1.0f, 0.0f);
  particle_a->mass() = 10.0f;

  particle_b->position() = Vector3f(0.1f, 3.0f, 0.0f);
  particle_b->mass() = 5.0f;

  particle_c->position() = Vector3f(1.0, 3.0f, 0.0f);
  particle_c->mass() = 2.0f;

  particle_d->position() = Vector3f(0.0f, 3.0f, 1.0f);
  particle_d->velocity() = Vector3f(0.0f, 1.0f, 0.0f);
  particle_d->mass() = 3.0f;

  ParticleRod *rod_a_b = new ParticleRod(particle_a, particle_b, 1.0f);
  ParticleRod *rod_b_c = new ParticleRod(particle_b, particle_c, 1.0f);
  ParticleRod *rod_b_d = new ParticleRod(particle_b, particle_d, 1.0f);

  system.AddContactGenerator(rod_a_b);
  system.AddContactGenerator(rod_b_c);
  system.AddContactGenerator(rod_b_d);
}

void TetrahedronSimulation::Reset() {
  Random generator;

  ParticleSystem &system = getSystem();

  // Note - these are leaking!
  ParticleGravityForce *gravity = new ParticleGravityForce();
  ParticleDampingForce *damping = new ParticleDampingForce(0.05, 0.01);
  ParticleAnchoredSpringForce *spring = new ParticleAnchoredSpringForce(
      Vector3f(0, 0, 0), 1, 500);
  system.AddForceGenerator(gravity);
  system.AddForceGenerator(damping);
  system.AddForceGenerator(spring);

  ParticleGroundContactGenerator *ground = new ParticleGroundContactGenerator();
  system.AddContactGenerator(ground);

  Particle *particles[4];
  for (int x = 0; x < 4; ++x) {
    particles[x] = system.NewParticle();
    particles[x]->position() = Vector3f(1.0f - x, 1.0f + 2 * x, x);
    particles[x]->mass() = (x + 1) * 10.0f;
    //particles[x]->velocity() = Vector3f(0.5 * x, 0.0f, 0.0f);

   system.LinkForceGenerator(particles[x], gravity);
   system.LinkForceGenerator(particles[x], damping);
  }

  particles[0]->velocity() = Vector3f(0.0f, 1.0f, 0.0f);
  particles[3]->velocity() = Vector3f(10.f, 0.0f, 0.0f);
  system.LinkForceGenerator(particles[0], spring);

  for (int x = 0; x < 4; ++x) {
    for (int y = x + 1; y < 4; ++y) {
      ParticleRod *rod = new ParticleRod(particles[x], particles[y], 2.0f);
      system.AddContactGenerator(rod);
    }
  }
}
