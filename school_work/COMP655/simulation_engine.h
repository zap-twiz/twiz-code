#ifndef INCLUDED_SIMULATION_ENGINE_H__
#define INCLUDED_SIMULATION_ENGINE_H__

#include "process_environment.h"

class SimulationBuilder {
 public:
  virtual ~SimulationBuilder() {}
  virtual void BuildSimulation(ProcessEnvironment* env) = 0;
  virtual void PrimeSimulation(ProcessEnvironment* env) = 0;
};

class SimulationEngine {
 public:
  SimulationEngine() {}

  void Init(SimulationBuilder* builder) {
    builder->BuildSimulation(&env_);
    builder->PrimeSimulation(&env_);
  }

  void TimeStep();

  bool IsIdle() const {
    return environment()->Idle(); 
  }

  ProcessEnvironment* environment() { return &env_; }
  ProcessEnvironment const * environment() const { return &env_; }

 private:
  ProcessEnvironment env_;
};

#endif  // INCLUDED_SIMULATION_ENGINE_H__