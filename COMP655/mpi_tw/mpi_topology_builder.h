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
    PartitionedTopologyBuilder::BuildInputPartition(engine);
    MPIPostMaster* post_master = static_cast<MPIPostMaster*>(post_master_);

    // The end cap has id 100, and is in proc with rank 1
    post_master->RegisterLPRank(100, 1);
  }

  virtual void BuildOutputPartition(SimulationEngine* engine) {
    PartitionedTopologyBuilder::BuildOutputPartition(engine);
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
