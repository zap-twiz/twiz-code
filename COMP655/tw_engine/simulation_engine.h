#ifndef INCLUDED_SIMULATION_ENGINE_H__
#define INCLUDED_SIMULATION_ENGINE_H__

#include "event.h"
#include "process_environment.h"
#include "post_master.h"

class SimulationEngine;
class SimulationBuilder {
 public:
  virtual ~SimulationBuilder() {}
  virtual void BuildSimulation(SimulationEngine* env) = 0;
  virtual void PrimeSimulation(ProcessEnvironment* env) = 0;
};

class SimulationEngine {
 public:
  SimulationEngine() : post_master_(NULL) {}

  void set_postmaster(PostMaster* post_master) {
    post_master_ = post_master;
    env_.set_postmaster(post_master);
  }

  void Init(SimulationBuilder* builder) {
    builder->BuildSimulation(this);
    builder->PrimeSimulation(&env_);
  }

  void TimeStep();

  bool IsIdle() const {
    return environment()->IsIdle(); 
  }

  // Note that this is the MINIMUM virtual time
  Time LocalVirtualTime() const;
  void ReceiveGlobalVirtualTime(Time gvt);

  ProcessEnvironment* environment() { return &env_; }
  ProcessEnvironment const * environment() const { return &env_; }

 private:
  ProcessEnvironment env_;
  PostMaster* post_master_;
};

#endif  // INCLUDED_SIMULATION_ENGINE_H__