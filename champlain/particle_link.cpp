
#include "particle_link.h"

#include "particle.h"
#include "particle_system.h"

#include "vector3f.h"

float ParticleLink::CurrentLength() const {
  if (!particle_a_ || !particle_b_) {
    return 0.0f;
  }

  Vector3f length = particle_a_->position();
  length -= particle_b_->position();

  return length.length();
}

void ParticleRod::AddContacts(ParticleSystem& system) {

  float current_length = CurrentLength();

  // No collision to add
  if (current_length == length_) {
    return;
  }

  Vector3f rod = particle_b_->position();
  rod -= particle_a_->position();
  rod.normalize();

  float depth = current_length - length_;
  if (current_length < length_) {
    depth = -depth;
    rod *= -1.0f;
  }
  ParticleContact contact(particle_a_,
                          particle_b_,
                          0, // restitution
                          depth,
                          rod);

  // Add A collision!
  system.AddContact(contact);

}
