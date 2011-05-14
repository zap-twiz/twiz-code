#ifndef INCLUDED_PARTITIONED_BUILDER_H__
#define INCLUDED_PARTITIONED_BUILDER_H__

#include "basic_processes.h"
#include "log_manager.h"
#include "simulation_engine.h"

class PartitionedTopologyBuilder : public SimulationBuilder {
 public:
  PartitionedTopologyBuilder(int id,
                             PartitionedPostMaster* post_master)
      : id_(id), post_master_(post_master) {}

  GeneratorProcess* generator1() { return input_1_; }

  virtual void BuildSimulation(SimulationEngine* engine);
  virtual void PrimeSimulation(ProcessEnvironment* env);
 protected:
  virtual void BuildInputPartition(SimulationEngine* engine);
  virtual void BuildOutputPartition(SimulationEngine* engine);
  virtual void PrimeInputPartition(ProcessEnvironment* env);

  virtual void PrimeOutputPartition(ProcessEnvironment* env) {
    // nothing to do!
  }

  int rank() { return id_; }

  int id_;
  PartitionedPostMaster* post_master_;

  GeneratorProcess *input_1_, *input_2_, *input_3_;
  PipelineProcess *middle_pipe_;
  ConsumerProcess *end_cap_;

  LogManager log_manager_;

  // Disallow copy & assign
  PartitionedTopologyBuilder() {}
  PartitionedTopologyBuilder(PartitionedTopologyBuilder const &) {}
};

#endif  // INCLUDED_PARTITIONED_BUILDER_H__
