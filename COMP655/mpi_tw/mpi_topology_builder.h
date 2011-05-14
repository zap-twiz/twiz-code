#ifndef INCLUDED_MPI_TOPOLOGY_BUILDER_H_
#define INCLUDED_MPI_TOPOLOGY_BUILDER_H_

#include "simulation_engine.h"
#include "partitioned_builder.h"

class MPITopologyBuilder : public PartitionedTopologyBuilder {
 public:
  MPITopologyBuilder(int rank, MPIPostMaster* post_master)
      : PartitionedTopologyBuilder(rank, post_master) {};

 protected:
  virtual void BuildInputPartition(SimulationEngine* engine) {
    ProcessEnvironment* env = engine->environment();
    MPIPostMaster* post_master = static_cast<MPIPostMaster*>(post_master_);

    int id = 0;
    RandomConstant<1> random;
    input_1_ = new GeneratorProcess(++id, &random);
    input_1_->set_name("Input 1");
    env->RegisterLogicalProcess(input_1_);
    post_master->RegisterRemoteLP(input_1_->id(), engine);
    input_2_ = new GeneratorProcess(++id, &random);
    input_2_->set_name("Input 2");
    env->RegisterLogicalProcess(input_2_);
    post_master->RegisterRemoteLP(input_2_->id(), engine);

    middle_pipe_ = new PipelineProcess(++id, &random);
    middle_pipe_->set_name("Middle Pipe");
    env->RegisterLogicalProcess(middle_pipe_);
    post_master->RegisterRemoteLP(middle_pipe_->id(), engine);

    input_1_->set_target(middle_pipe_->id());
    input_1_->set_count(20000);

    input_2_->set_target(middle_pipe_->id());
    input_2_->set_count(20000);

    middle_pipe_->set_target(100);

    // The end cap has id 100, and is in proc with rank 1
    post_master->RegisterLPRank(100, 1);
  }

  virtual void BuildOutputPartition(SimulationEngine* engine) {
    ProcessEnvironment* env = engine->environment();
    MPIPostMaster* post_master = static_cast<MPIPostMaster*>(post_master_);

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
};

#endif  // INCLUDED_MPI_TOPOLOGY_BUILDER_H_
