
#include "particle_contact.h"

#include "particle.h"

#include <limits>

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

  if (separating_velocity >= 0) {
    // Particles are separating
    return;
  }

  Vector3f acceleration = actorA_->acceleration();
  if (actorB_) {
    acceleration -= actorB_->acceleration();
  }


  float new_velocity = separating_velocity;
  float accumulated_velocity = normal_.dot(acceleration * duration);
  if (separating_velocity != accumulated_velocity) {
//    std::cout << "Velocity Diff!" << std::endl;
  }

  if (accumulated_velocity < 0) {
    new_velocity -= accumulated_velocity;
  }

  new_velocity = -new_velocity * restitution_;

  float delta_velocity = new_velocity - separating_velocity;

  float total_inverse_mass = actorA_->inverse_mass();
  if (actorB_) {
    total_inverse_mass += actorB_->inverse_mass();
  }

  // Two objects of infinite mass. Unable to process the collision.
  if (total_inverse_mass == 0.0f) {
    return;
  }

  float impulse = delta_velocity / total_inverse_mass;

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

void NaiveParticleContactResolver::ResolveContacts(ParticleContact *contacts, int num_contacts,
                                                   float duration) {
  for (int i = 0; i < num_contacts; ++i) {
    contacts[i].Resolve(duration);
  }
}

#include <iostream>

void MultiPassParticleContactResolver::ResolveContacts(ParticleContact *contacts, int num_contacts,
                                                       float duration) {
  int processed = 0;
  while (processed < passes_) {
    float min_separating_velocity = std::numeric_limits<float>::max();
    int candidate = 0;
    for (int x = 0; x < num_contacts; ++x) {
      // Find the collision of highest separating velocity
      float separating_velocity = contacts[x].SeparatingVelocity();

      if (separating_velocity <= min_separating_velocity &&
          (separating_velocity < 0.0f || contacts[x].depth() > 0)) {
        min_separating_velocity = separating_velocity;
        candidate = x;
      }
    }

    // No collision found
    if (min_separating_velocity > 0.0f) {
      break;
    }

    //std::cout << "Resolving " << candidate << std::endl;

    Particle *actor_a = contacts[candidate].actorA();
    Particle *actor_b = contacts[candidate].actorB();

    Vector3f const & move_a = contacts[candidate].actor_A_displacement();
    Vector3f const & move_b = contacts[candidate].actor_B_displacement();

    // Process the best candidate
    contacts[candidate].Resolve(duration);
    // Update all of the other collisions
    for (int x = 0; x < num_contacts; ++x) {
      // Note - this loop includes the contact that was just resolved!

      // Update the depth of the relevant contacts ...
      ParticleContact *contact = &contacts[x];
      if (contact->actorA() == actor_a) {
        contact->set_depth(contact->depth() - move_a.dot(contact->normal()));
      }

      if (contact->actorA() == actor_b) {
        contact->set_depth(contact->depth() - move_b.dot(contact->normal()));
      }

      if (contact->actorB()) {
        if (contact->actorB() == actor_a) {
          contact->set_depth(contact->depth() + move_a.dot(contact->normal()));
        }

        if (contact->actorB() == actor_b) {
          contact->set_depth(contact->depth() + move_b.dot(contact->normal()));
        }
      }
    }

    ++processed;
  }
}
