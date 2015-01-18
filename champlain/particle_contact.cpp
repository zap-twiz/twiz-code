
#include "particle_contact.h"

#include "particle.h"

ParticleContact::ParticleContact(Particle *actorA,
                                 Particle *actorB,
                                 float restitution,
                                 float depth,
                                 Vector3f const &normal)
  : actorA_(actorA),
    actorB_(actorB),
    restitution_(restitution),
    depth_(depth),
    normal_(normal) {
}

float ParticleContact::SeparatingVelocity() const {
  Vector3f relative_velocity = actorA_->velocity();
  if (actorB_) {
    relative_velocity -= actorB_->velocity();
  }
  return relative_velocity.dot(normal_);
}

void ParticleContact::Resolve(float duration) {
  ResolveVelocity(duration);
  ResolveInterpenetration(duration);
}

void ParticleContact::ResolveVelocity(float duration) {

  float separating_velocity = SeparatingVelocity();

  if (separating_velocity > 0) {
    // Particles are separating
    return;
  }

  float new_velocity = -separating_velocity * restitution_;

  float delta_velocity = new_velocity - separating_velocity;


  float total_inverse_mass = actorA_->inverse_mass();
  if (actorB_) {
    total_inverse_mass += actorB_->inverse_mass();
  }

  // Two objects of infinite mass. Unable to process the collision.
  if (total_inverse_mass == 0.0f) {
    return;
  }

  float impulse = delta_velocity * total_inverse_mass;

  Vector3f directed_impulse = normal_ * impulse;

  actorA_->velocity() += actorA_->inverse_mass() * directed_impulse;
  if (actorB_) {
    actorB_->velocity() -= actorB_->inverse_mass() * directed_impulse;
  }
}

void ParticleContact::ResolveInterpenetration(float duration) {
  // No depth to restore
  if (depth_ <= 0.0f) {
    return;
  }

  float total_inverse_mass = actorA_->inverse_mass();
  if (actorB_) {
    total_inverse_mass += actorB_->inverse_mass();
  }

  if (total_inverse_mass == 0.0f) {
    // Two objects of infinite mass.
    return;
  }

  Vector3f directed_displacement = normal_ * (depth_ / total_inverse_mass);

  actor_a_displacement_ = directed_displacement * actorA_->inverse_mass();
  if (actorB_) {
    actor_b_displacement_ = -1.0 * directed_displacement * actorB_->inverse_mass();
  } else {
    actor_b_displacement_ = Vector3f(0.0f, 0.0f, 0.0f);
  }

  actorA_->position() += actor_a_displacement_;
  if (actorB_) {
    actorB_->position() += actor_b_displacement_;
  }
}

void NaiveParticleContactResolver::ResolveContacts(ParticleContact *contacts, int num_contacts) {
  for (int i = 0; i < num_contacts; ++i) {
    //contacts[i].Resolve();
  }
}
