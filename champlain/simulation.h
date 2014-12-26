#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>

class Particle;
class ParticleSystem;
class ParticleForceGenerator;

class Simulation {
 public:
  virtual ~Simulation();

  virtual void Reset() = 0;
  virtual void Pause() = 0;
  virtual void Step(float duration);

  virtual ParticleSystem& getSystem() = 0;

  ParticleForceGenerator* AddForceGenerator(ParticleForceGenerator* generator);
  void LinkForceGenerator(Particle* particle, ParticleForceGenerator* generator);

 protected:
  struct ForceRegistration {
    Particle* particle;
    ParticleForceGenerator* generator;
  };

  std::vector<ForceRegistration> forceMapping_;
  std::vector<ParticleForceGenerator*> forceGenerators_;
};

#include "particle.h"

class FireworksSimulation : public Simulation{
 public:
  FireworksSimulation() {}
  virtual ~FireworksSimulation() {}

  virtual void Reset();
  virtual void Pause();
  virtual void Step(float duration);

  virtual ParticleSystem& getSystem() { return system_; }

 private:
  ParticleSystem system_;
};

#endif // SIMULATION_H

