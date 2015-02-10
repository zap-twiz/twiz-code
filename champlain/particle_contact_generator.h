#ifndef PARTICLE_CONTACT_GENERATOR_H
#define PARTICLE_CONTACT_GENERATOR_H

class ParticleSystem;
class ParticleContactGenerator {
 public:
  virtual ~ParticleContactGenerator() {}

  virtual void AddContacts(ParticleSystem& system) = 0;
};

class ParticleGroundContactGenerator : public ParticleContactGenerator {
 public:
  virtual void AddContacts(ParticleSystem& system);
};

#endif // PARTICLE_CONTACT_GENERATOR_H

