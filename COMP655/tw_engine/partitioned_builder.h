#ifndef INCLUDED_PARTITIONED_BUILDER_H__
#define INCLUDED_PARTITIONED_BUILDER_H__

#include "basic_processes.h"
#include "log_manager.h"
#include "simulation_engine.h"

class PartitionedPostMaster;

// Class implementing the LP network construction interface of
// |SimulationBuilder|.  This class is capable of building
// two differnt simulations that can interact with each-other
// via message passing.
class PartitionedTopologyBuilder : public SimulationBuilder {
 public:
  // Construct with |id| representing the id of the partition to build.
  // id = 0 ->  Build the left-half of the assigned simulation.
  // id = 1 ->  Build the right-half of the assigned simulation.
  // |post_master| 
  PartitionedTopologyBuilder(int id,
                             PartitionedPostMaster* post_master)
      : id_(id), post_master_(post_master) {}

  // Accessor method for the first generator process.
  // Use the returned LP to inject messages into the system.
  GeneratorProcess* generator1() { return input_1_; }

  // Builds the simulation inside |engine|.
  virtual void BuildSimulation(SimulationEngine* engine);

  // Prepares the simulation for execution.  Primes the input queues
  // of the input LPs.
  virtual void PrimeSimulation(ProcessEnvironment* env);
 protected:

  // Build the left-half of the assigned model.
  virtual void BuildInputPartition(SimulationEngine* engine);

  // Build the right-half of the assigned model.
  virtual void BuildOutputPartition(SimulationEngine* engine);

  // Prepare the left and right halves of the simulation model, after
  // construction.
  virtual void PrimeInputPartition(ProcessEnvironment* env);
  virtual void PrimeOutputPartition(ProcessEnvironment* env) {
    // nothing to do!
  }

  int rank() { return id_; }

  int id_;

  // The post-master that will be populated with the topology information
  // of the simulation.
  PartitionedPostMaster* post_master_;

  // Handles to the various LPs built by the builder.
  GeneratorProcess *input_1_, *input_2_, *input_3_;
  PipelineProcess *middle_pipe_;
  ConsumerProcess *end_cap_;

  // The builder manages the lifetime of the logs used for the LPs.
  // The builder should live at least as long as the simulation
  // environment that it constructs.
  LogManager log_manager_;

  // Disallow copy & assign
  PartitionedTopologyBuilder() {}
  PartitionedTopologyBuilder(PartitionedTopologyBuilder const &) {}
};

#endif  // INCLUDED_PARTITIONED_BUILDER_H__
