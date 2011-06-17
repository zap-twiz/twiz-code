// heap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "heap.h"

#include <functional>
#include <iostream>

using namespace std;

#include "simulation_engine.h"
#include "basic_processes.h"

class BasicTopologyBuilder : public SimulationBuilder {
 public:
  GeneratorProcess* generator1() { return input_1_; }
  GeneratorProcess* generator2() { return input_2_; }

  virtual void BuildSimulation(ProcessEnvironment* env) {

    RandomConstant<1> random;
    input_1_ = new GeneratorProcess(env->next_id(), &random);
    input_1_->set_name("Input 1");
    env->RegisterLogicalProcess(input_1_);
#if 1
    input_2_ = new GeneratorProcess(env->next_id(), &random);
    input_2_->set_name("Input 2");
    env->RegisterLogicalProcess(input_2_);
#endif

    input_3_ = new GeneratorProcess(env->next_id(), &random);
    input_3_->set_name("Input 3");
    env->RegisterLogicalProcess(input_3_);

    middle_pipe_ = new PipelineProcess(env->next_id(), &random);
    middle_pipe_->set_name("Middle Pipe");
    env->RegisterLogicalProcess(middle_pipe_);

    end_cap_ = new ConsumerProcess(env->next_id());
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
  }
 private:
  GeneratorProcess *input_1_, *input_2_, *input_3_;
  PipelineProcess *middle_pipe_;
  ConsumerProcess *end_cap_;
};

int _tmain(int argc, _TCHAR* argv[]) {

  SimulationEngine engine;
  BasicTopologyBuilder simple_builder;

  engine.Init(&simple_builder);

  int step_count = 0;
  while (step_count < 10) {
    engine.TimeStep();
    ++step_count;

    if (step_count == 7) {
      Event bogus_event;
      bogus_event.set_receive_time_stamp(100);
      bogus_event.set_send_time_stamp(100);
      bogus_event.set_sending_process_id(-1);
      bogus_event.set_payload(-1);
      bogus_event.set_type(47);
      bogus_event.set_target_process_id(simple_builder.generator1()->id());
      engine.environment()->Send(bogus_event);
    }
  }

  //simple_builder.generator1()->Inhibit();
  //simple_builder.generator2()->Inhibit();

  while (!engine.IsIdle()) {
    engine.TimeStep();
  }

#if 1
  Event bogus_event;
  bogus_event.set_receive_time_stamp(0);
  bogus_event.set_send_time_stamp(0);
  bogus_event.set_sending_process_id(-1);
  bogus_event.set_payload(20000);
  bogus_event.set_type(666);
  bogus_event.set_target_process_id(simple_builder.generator2()->id());
  engine.environment()->Send(bogus_event);
#endif

  while (!engine.IsIdle()) {
    engine.TimeStep();
  }

  return 0;
}

