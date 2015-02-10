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
};

#include "particle.h"
#include "particle_system.h"

class BaseSimulation : public Simulation {
 public:
  BaseSimulation();
  virtual ~BaseSimulation();

  virtual void Pause();

  virtual ParticleSystem& getSystem() { return system_; }

 protected:
  ParticleSystem system_;
  ParticleContactResolver* resolver_;
};

class ParticleContactResolver;

class FireworksSimulation : public BaseSimulation {
 public:
  virtual void Reset();
};

class SimpleGravitySimulation : public BaseSimulation {
 public:
  virtual void Reset();
};

class MultiRodSimulation : public BaseSimulation {
 public:
  virtual void Reset();
};

class TetrahedronSimulation : public BaseSimulation {
 public:
  virtual void Reset();
};

#endif // SIMULATION_H

