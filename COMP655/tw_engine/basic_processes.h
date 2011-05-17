#ifndef INCLUDED_BASIC_PROCESSES_H__
#define INCLUDED_BASIC_PROCESSES_H__

#include "event.h"
#include "logical_process.h"
#include "process_environment.h"
#include "random.h"

#include <iostream>

// This file defines a set of LPs mapping to those given in the assignment.

// A pipeline LP.  This LP receives events, and carries them forward to a
// target LP.  Processing delay is also introduced to the event time-stamps.
class PipelineProcess : public LogicalProcess {
 public:
  // State memento for pipeline LPs.
  class PipelineProcessState : public State {
   public:
    PipelineProcessState(Time time, Random* generator)
        : State(time), random_(generator) {}

    Random* get_random() { return random_.release(); }
   private:
    std::auto_ptr<Random> random_;
  };

  PipelineProcess(int id, Random* random)
      : LogicalProcess(id), generator_(random->clone()) {}
  virtual ~PipelineProcess() {
    delete generator_;
  }

  // Assign the target LP to which this LP will send events.
  void set_target(int target) { target_lp_ = target; }
  
  virtual Time Evaluate(Event const & event,
                        ProcessEnvironment* process_environment);

 protected:
  virtual State* BuildMemento() {
    return new PipelineProcessState(LogicalTime(), generator_->clone());
  }

  // Reconstruct a pipeline LP, given a State memento.
  virtual void ResurrectMemento(State* state) {
    LogicalProcess::ResurrectMemento(state);
    PipelineProcessState* pipeline_state =
        static_cast<PipelineProcessState*>(state);
    delete generator_;
    generator_ = pipeline_state->get_random();
  }

  Random* generator() { return generator_; }
 private:
  int target_lp_;
  Random* generator_;
};

inline Time PipelineProcess::Evaluate(Event const & event,
                                      ProcessEnvironment* process_environment) {
  // Generate a random processing time.
  unsigned int process_time = generator_->Generate();
  unsigned int send_time = 0;/*generator_->Generate();*/

  // Construct a new event, incrementing the payload type.
  // Note:  The increment is arbitrary.
  Event new_event;
  new_event.set_payload(event.payload() + 1);

  new_event.set_target_process_id(target_lp_);
  new_event.set_sending_process_id(id());

  // Factor in the processing time to the time of the event.
  new_event.set_send_time_stamp(event.receive_time_stamp() + process_time);
  new_event.set_receive_time_stamp(new_event.send_time_stamp() + send_time);

  new_event.set_type(event.type());

  SendEvent(new_event, process_environment);
  return event.receive_time_stamp();
}

// A consumer LP class.  The consumer is the LP that receives messages
// from the input portion of the simulation.  A consumer does not produce
// any output messages.
class ConsumerProcess : public LogicalProcess {
 public:
  ConsumerProcess(int id) : LogicalProcess(id) {}
  virtual Time Evaluate(Event const & event,
                        ProcessEnvironment* process_environment) {
    return event.receive_time_stamp();
  }

 protected:
  // No state to capture, other than the LP time.
  virtual State* BuildMemento() { return new State(LogicalTime()); }
  virtual void ResurrectMemento(State* state) {
    LogicalProcess::ResurrectMemento(state);
  }
};

// A GeneratorProcess is an LP that sends messages both to a target process,
// and back to itself.  By doing-so, it creates an endless stream of messages.
class GeneratorProcess : public PipelineProcess {
 public:
  // Memento for the state of a generator process.  Tracks the number
  // of messages forwarded.
  class GeneratorProcessState : public PipelineProcessState {
   public:
    GeneratorProcessState(int count, Time time, Random* random)
        : PipelineProcessState(time, random), count_(count) {}
    int event_count() const { return count_; }
   private:
    int count_;
  };

  GeneratorProcess(int id, Random* random)
      : PipelineProcess(id, random), event_count_(0) {}

  // Assign the number of messages to be sent by the generator.
  // Only |count| messages will be generated.
  void set_count(int count) { event_count_ = count; }

  virtual Time Evaluate(Event const & event,
                        ProcessEnvironment* process_environment) {
    Time time = PipelineProcess::Evaluate(event, process_environment);
    if (0 == event_count_)
      return time;

    Event new_event;
    new_event.set_payload(event.payload() + 1);

    new_event.set_target_process_id(id());
    new_event.set_sending_process_id(id());

    new_event.set_send_time_stamp(event.receive_time_stamp() + 1);
    new_event.set_receive_time_stamp(new_event.send_time_stamp());

    new_event.set_type(event.type());

    SendEvent(new_event, process_environment);

    --event_count_;
    return event.receive_time_stamp();
  }

 protected:
  virtual State* BuildMemento() {
    return new GeneratorProcessState(event_count_, LogicalTime(),
                                     generator()->clone());
  }

  // Restore the state of the generator process.
  virtual void ResurrectMemento(State* state) {
    PipelineProcess::ResurrectMemento(state);
    GeneratorProcessState* generator_state =
        static_cast<GeneratorProcessState*>(state);
    // Reset the count to reflect the previous state.
    event_count_ = generator_state->event_count();
  }

 private:
  int event_count_;
};

#endif  // INCLUDED_BASIC_PROCESSES_H__
