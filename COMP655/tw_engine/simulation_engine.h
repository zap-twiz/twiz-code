#ifndef INCLUDED_SIMULATION_ENGINE_H__
#define INCLUDED_SIMULATION_ENGINE_H__

#include "event.h"
#include "named_entity.h"
#include "process_environment.h"
#include "post_master.h"

class SimulationEngine;
class SimulationBuilder {
 public:
  virtual ~SimulationBuilder() {}
  virtual void BuildSimulation(SimulationEngine* env) = 0;
  virtual void PrimeSimulation(ProcessEnvironment* env) = 0;
};

class SimulationEngine : public NamedEntity {
 public:
  SimulationEngine() : post_master_(NULL) {}

  // Non-owned
  void set_postmaster(PostMaster* post_master) {
    post_master_ = post_master;
    env_.set_postmaster(post_master);
  }
  PostMaster* get_postmaster() const { return post_master_; }

  void Init(SimulationBuilder* builder) {
    builder->BuildSimulation(this);
    builder->PrimeSimulation(&env_);
  }

  void TimeStep();

  bool IsIdle() const {
    return environment()->IsIdle(); 
  }

  bool Finished() const;

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
