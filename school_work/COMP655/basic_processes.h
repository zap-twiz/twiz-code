#ifndef INCLUDED_BASIC_PROCESSES_H__
#define INCLUDED_BASIC_PROCESSES_H__

#include "event.h"
#include "logical_process.h"
#include "process_environment.h"
#include "random.h"

class PipelineProcess : public LogicalProcess {
 public:
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

  void set_target(int target) { target_lp_ = target; }
  
  virtual Time Evaluate(Event const & event,
                        ProcessEnvironment* process_environment);

 protected:
  virtual State* BuildMemento() {
    return new PipelineProcessState(LogicalTime(), generator_->clone());
  }

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
  unsigned int process_time = generator_->Generate();
  unsigned int send_time = 0;/*generator_->Generate();*/

  Event new_event;
  new_event.set_payload(event.payload() + 1);

  new_event.set_target_process_id(target_lp_);
  new_event.set_sending_process_id(id());

  new_event.set_send_time_stamp(event.receive_time_stamp() + process_time);
  new_event.set_receive_time_stamp(new_event.send_time_stamp() + send_time);

  new_event.set_type(event.type());

  SendEvent(new_event, process_environment);
  return event.receive_time_stamp();
}

class ConsumerProcess : public LogicalProcess {
 public:
  ConsumerProcess(int id) : LogicalProcess(id) {}
  virtual Time Evaluate(Event const & event,
                        ProcessEnvironment* process_environment) {
    std::cout << "Consumer Received:  " << event.receive_time_stamp() <<
        " data: " << event.payload() << " type: " << event.type() << std::endl;
    return event.receive_time_stamp();
  }

 protected:
  // No state to capture
  virtual State* BuildMemento() { return new State(LogicalTime()); }
  virtual void ResurrectMemento(State* state) {
    LogicalProcess::ResurrectMemento(state);
    /* nothing to do! */
  }
};

class GeneratorProcess : public PipelineProcess {
 public:
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

  virtual void ResurrectMemento(State* state) {
    PipelineProcess::ResurrectMemento(state);
    GeneratorProcessState* generator_state =
        static_cast<GeneratorProcessState*>(state);
    event_count_ = generator_state->event_count();
  }

 private:
  int event_count_;
};

#endif  // INCLUDED_BASIC_PROCESSES_H__
