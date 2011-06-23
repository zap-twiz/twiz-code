#ifndef INCLUDED_SIMULATION_ENGINE_H__
#define INCLUDED_SIMULATION_ENGINE_H__

#include "event.h"
#include "named_entity.h"
#include "process_environment.h"
#include "post_master.h"

class SimulationEngine;

// A factory interface for building simulation environments.
// Pass implementations to |SimulationEngine| to construct
// different simulation topologies.
class SimulationBuilder {
 public:
  virtual ~SimulationBuilder() {}

  // Interface point called to build the simulatoin.  All LPs, and
  // post-master construction should take place here.
  virtual void BuildSimulation(SimulationEngine* env) = 0;

  // Routine called to prepare a simulation for execution.
  // Called to populate the system with initial messages.
  virtual void PrimeSimulation(ProcessEnvironment* env) = 0;
};

class SimulationEngine : public NamedEntity {
 public:
  SimulationEngine() : post_master_(NULL) {}

  // Assign the post-master for the simulation engine.  The engine
  // does not own the post-master.
  void set_postmaster(PostMaster* post_master) {
    post_master_ = post_master;
    env_.set_postmaster(post_master);
  }
  PostMaster* get_postmaster() const { return post_master_; }

  // Initializer routine that uses |builder| to construct the simulation.
  void Init(SimulationBuilder* builder) {
    builder->BuildSimulation(this);
    builder->PrimeSimulation(&env_);
  }

  // The main work-horse of the system.  Called repeatedly to proces
  // events and move the simulation forward.
  void TimeStep();

  // Call to determine if there is any remaining work internal to the engine.
  // If the input queues are empty, then the simulation is idle.
  bool IsIdle() const {
    return environment()->IsIdle(); 
  }

  // TODO:  Not implemented.
  bool Finished() const;

  // Note that this is the MINIMUM virtual time
  Time LocalVirtualTime() const;

  // Invoke to inform the engine of the global veritual time of the simulation
  // as a whole.
  void ReceiveGlobalVirtualTime(Time gvt);

  // Accessors for the |ProcessEnvironment| hosted in the engine.
  ProcessEnvironment* environment() { return &env_; }
  ProcessEnvironment const * environment() const { return &env_; }

 private:
  ProcessEnvironment env_;
  PostMaster* post_master_;
};

#endif  // INCLUDED_SIMULATION_ENGINE_H__
