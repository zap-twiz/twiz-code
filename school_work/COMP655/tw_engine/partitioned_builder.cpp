
#include "partitioned_builder.h"
#include "partitioned_post_master.h"

#include <assert.h>

void PartitionedTopologyBuilder::BuildSimulation(SimulationEngine* engine) {
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

void PartitionedTopologyBuilder::PrimeSimulation(ProcessEnvironment* env) {
  switch (rank()) {
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

void PartitionedTopologyBuilder::BuildInputPartition(SimulationEngine* engine) {
  ProcessEnvironment* env = engine->environment();

  int id = 0;
  RandomConstant<1> random;
  input_1_ = new GeneratorProcess(++id, &random);
  input_1_->set_name("Input 1");
  env->RegisterLogicalProcess(input_1_);
  post_master_->RegisterRemoteLP(input_1_->id(), engine);
  Logger* logger_1 = new Logger(InitOutputStream(input_1_->name().c_str()));
  input_1_->set_logger(logger_1);
  log_manager_.RegisterLogger(logger_1);


  input_2_ = new GeneratorProcess(++id, &random);
  input_2_->set_name("Input 2");
  env->RegisterLogicalProcess(input_2_);
  post_master_->RegisterRemoteLP(input_2_->id(), engine);
  Logger* logger_2 = new Logger(InitOutputStream(input_2_->name().c_str()));
  input_2_->set_logger(logger_2);
  log_manager_.RegisterLogger(logger_2);

  middle_pipe_ = new PipelineProcess(++id, &random);
  middle_pipe_->set_name("Middle Pipe");
  env->RegisterLogicalProcess(middle_pipe_);
  post_master_->RegisterRemoteLP(middle_pipe_->id(), engine);
  Logger* logger_m =
      new Logger(InitOutputStream(middle_pipe_->name().c_str()));
  middle_pipe_->set_logger(logger_m);
  log_manager_.RegisterLogger(logger_m);


  input_1_->set_target(middle_pipe_->id());
  input_1_->set_count(20000);

  input_2_->set_target(middle_pipe_->id());
  input_2_->set_count(20000);

  middle_pipe_->set_target(100);
}

void PartitionedTopologyBuilder::BuildOutputPartition(SimulationEngine* engine) {
  ProcessEnvironment* env = engine->environment();
  int id = 100;

  end_cap_ = new ConsumerProcess(id);
  end_cap_->set_name("End Cap");
  env->RegisterLogicalProcess(end_cap_);
  post_master_->RegisterRemoteLP(end_cap_->id(), engine);
  Logger* logger_end =
      new Logger(InitOutputStream(end_cap_->name().c_str()));
  end_cap_->set_logger(logger_end);
  log_manager_.RegisterLogger(logger_end);
}

void PartitionedTopologyBuilder::PrimeInputPartition(ProcessEnvironment* env) {
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
