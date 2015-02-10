
#include "particle_contact_generator.h"

#include "particle_system.h"

void ParticleGroundContactGenerator::AddContacts(ParticleSystem& system) {
  std::vector<Particle>& particles = system.particles();

  for (Particle& particle : particles) {

    // Naive ground contact generation.
    if (particle.position().y() <= 0.0f) {
      ParticleContact contact(&particle,
                              0,
                              1.5, // zero restitution
                              -particle.position().y(),
                              Vector3f(0.0f, 1.0f, 0.0f));
      system.AddContact(contact);
    }
  }
}
