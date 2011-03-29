#ifndef INCLUDED_BASIC_TOPOLOGY_BUILDER_H_
#define INCLUDED_BASIC_TOPOLOGY_BUILDER_H_

#include "basic_processes.h"
#include "simulation_engine.h"

#include <assert.h>
#include <vector>

class PartitionedTopologyBuilder : public SimulationBuilder {
 public:
  PartitionedTopologyBuilder(int id,
                             PartitionedPostMaster* post_master)
      : id_(id), post_master_(post_master) {}

  GeneratorProcess* generator1() { return input_1_; }

  virtual void BuildSimulation(SimulationEngine* engine) {
    switch (id_) {
      case 0:
        BuildInputPartition(engine);
        break;
      case 1:
        BuildOutputPartition(engine);
        break;
      default:
        assert(false);
    }
    engine->set_postmaster(post_master_);
  }

  virtual void PrimeSimulation(ProcessEnvironment* env) {
    switch (id_) {
      case 0:
        PrimeInputPartition(env);
        break;
      case 1:
        PrimeOutputPartition(env);
        break;
      default:
        assert(false);
    }
  }
 protected:
  virtual void BuildInputPartition(SimulationEngine* engine) {
    ProcessEnvironment* env = engine->environment();

    int id = 0;
    RandomConstant<1> random;
    input_1_ = new GeneratorProcess(++id, &random);
    input_1_->set_name("Input 1");
    env->RegisterLogicalProcess(input_1_);
    post_master_->RegisterRemoteLP(input_1_->id(), engine);
    input_2_ = new GeneratorProcess(++id, &random);
    input_2_->set_name("Input 2");
    env->RegisterLogicalProcess(input_2_);
    post_master_->RegisterRemoteLP(input_2_->id(), engine);

    middle_pipe_ = new PipelineProcess(++id, &random);
    middle_pipe_->set_name("Middle Pipe");
    env->RegisterLogicalProcess(middle_pipe_);
    post_master_->RegisterRemoteLP(middle_pipe_->id(), engine);

    input_1_->set_target(middle_pipe_->id());
    input_1_->set_count(20000);

    input_2_->set_target(middle_pipe_->id());
    input_2_->set_count(20000);

    middle_pipe_->set_target(100);
  }
  virtual void BuildOutputPartition(SimulationEngine* engine) {
    ProcessEnvironment* env = engine->environment();
    int id = 100;
    end_cap_ = new ConsumerProcess(id);
    end_cap_->set_name("End Cap");
    env->RegisterLogicalProcess(end_cap_);
    post_master_->RegisterRemoteLP(end_cap_->id(), engine);
  }

  virtual void PrimeInputPartition(ProcessEnvironment* env) {
    Event new_event;
    new_event.set_payload(0);
    new_event.set_type(42);

    new_event.set_send_time_stamp(1000);
    new_event.set_receive_time_stamp(1000);

    new_event.set_target_process_id(input_1_->id());
    new_event.set_sending_process_id(1234);
    env->Send(new_event);

    new_event.set_payload(10000);
    new_event.set_type(43);

    new_event.set_target_process_id(input_2_->id());
    new_event.set_sending_process_id(1234);

    env->Send(new_event);

  }
  virtual void PrimeOutputPartition(ProcessEnvironment* env) {
    // nothing to do!
  }

  int rank() { return id_; }

  int id_;
  PartitionedPostMaster* post_master_;

  GeneratorProcess *input_1_, *input_2_, *input_3_;
  PipelineProcess *middle_pipe_;
  ConsumerProcess *end_cap_;

  // Disallow copy & assign
  PartitionedTopologyBuilder() {}
  PartitionedTopologyBuilder(PartitionedTopologyBuilder const &) {}
};

#endif  // INCLUDED_BASIC_TOPOLOGY_BUILDER_H_
