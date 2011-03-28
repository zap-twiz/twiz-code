#ifndef INCLUDED_BASIC_TOPOLOGY_BUILDER_H_
#define INCLUDED_BASIC_TOPOLOGY_BUILDER_H_

#include "basic_processes.h"
#include "simulation_engine.h"

class BasicTopologyBuilder : public SimulationBuilder {
 public:
  GeneratorProcess* generator1() { return input_1_; }
  GeneratorProcess* generator2() { return input_2_; }

  virtual void BuildSimulation(SimulationEngine* engine) {
    ProcessEnvironment* env = engine->environment();

    int id = 0;
    RandomConstant<1> random;
    input_1_ = new GeneratorProcess(++id, &random);
    input_1_->set_name("Muffin Input 1");
    env->RegisterLogicalProcess(input_1_);
#if 1
    input_2_ = new GeneratorProcess(++id, &random);
    input_2_->set_name("Muffin Input 2");
    env->RegisterLogicalProcess(input_2_);
#endif

    input_3_ = new GeneratorProcess(++id, &random);
    input_3_->set_name("Muffin Input 3");
    env->RegisterLogicalProcess(input_3_);

    middle_pipe_ = new PipelineProcess(++id, &random);
    middle_pipe_->set_name("Middle Pipe");
    env->RegisterLogicalProcess(middle_pipe_);

    end_cap_ = new ConsumerProcess(++id);
    end_cap_->set_name("End Cap");
    env->RegisterLogicalProcess(end_cap_);

    input_1_->set_target(middle_pipe_->id());
    input_1_->set_count(30);

    input_2_->set_target(middle_pipe_->id());
    input_2_->set_count(30);

    input_3_->set_target(end_cap_->id());
    input_3_->set_count(30);

    middle_pipe_->set_target(end_cap_->id());
  }

  virtual void PrimeSimulation(ProcessEnvironment* env) {
    Event new_event;
    new_event.set_payload(0);
    new_event.set_type(42);

    new_event.set_send_time_stamp(1000);
    new_event.set_receive_time_stamp(1000);

    new_event.set_target_process_id(input_1_->id());
    new_event.set_sending_process_id(1234);

    env->Send(new_event);

#if 1
    new_event.set_payload(10000);
    new_event.set_type(43);

    new_event.set_target_process_id(input_2_->id());
    new_event.set_sending_process_id(1234);

    env->Send(new_event);

    new_event.set_payload(30000);
    new_event.set_type(44);

    new_event.set_target_process_id(input_3_->id());
    new_event.set_sending_process_id(1234);

    env->Send(new_event);
#endif
  }
 private:
  GeneratorProcess *input_1_, *input_2_, *input_3_;
  PipelineProcess *middle_pipe_;
  ConsumerProcess *end_cap_;
};

#include <assert.h>

#include <vector>

class PartitionedTopologyBuilder : public SimulationBuilder {
 public:
  PartitionedTopologyBuilder(int id,
                             std::vector<SimulationEngine>* engines,
                             PartitionedPostMaster* post_master)
      : id_(id), engines_(engines), post_master_(post_master) {}

  GeneratorProcess* generator1() { return input_1_; }

  virtual void BuildSimulation(SimulationEngine* engine) {
    assert(engine == &(*engines_)[id_]);
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
    //assert(engine == &(*engines_)[id_]);
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
 private:
  void BuildInputPartition(SimulationEngine* engine) {
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
    input_1_->set_count(3000);

    input_2_->set_target(middle_pipe_->id());
    input_2_->set_count(3000);

    middle_pipe_->set_target(100);
  }
  void BuildOutputPartition(SimulationEngine* engine) {
    ProcessEnvironment* env = engine->environment();
    int id = 100;
    end_cap_ = new ConsumerProcess(id);
    end_cap_->set_name("End Cap");
    env->RegisterLogicalProcess(end_cap_);
    post_master_->RegisterRemoteLP(end_cap_->id(), engine);
  }

  void PrimeInputPartition(ProcessEnvironment* env) {
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
  void PrimeOutputPartition(ProcessEnvironment* env) {}

  int id_;
  std::vector<SimulationEngine>* engines_;
  PartitionedPostMaster* post_master_;

  GeneratorProcess *input_1_, *input_2_, *input_3_;
  PipelineProcess *middle_pipe_;
  ConsumerProcess *end_cap_;

  // Disallow copy & assign
  PartitionedTopologyBuilder() {}
  PartitionedTopologyBuilder(PartitionedTopologyBuilder const &) {}
};

#endif  // INCLUDED_BASIC_TOPOLOGY_BUILDER_H_
