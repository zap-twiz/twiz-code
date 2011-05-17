#include "stdafx.h"

#include "logical_process.h"
#include "process_environment.h"

#include <algorithm>
#include <assert.h>
#include <iostream>

namespace {

// Basic, brute-force implementation of set-intersect.  Determines the set of
// of overlapping events in the inputs, |pos_events|, and |neg_events|.
void PositiveNegativeIntersect(std::vector<Event> const & pos_events,
                               std::vector<Event> const & neg_events,
                               std::vector<Event>* intersection ) {
  intersection->clear();

  // TODO:  Change this from an O(N^2) approach.
  std::vector<Event>::const_iterator neg_iter(neg_events.begin()),
    neg_end(neg_events.end());
  for (; neg_iter != neg_end; ++neg_iter) {
    Event positive_equivalent = *neg_iter;
    positive_equivalent.negate();

    std::vector<Event>::const_iterator found_pos = std::find(
        pos_events.begin(), pos_events.end(), positive_equivalent);
    if (found_pos != pos_events.end()) {
      intersection->push_back(positive_equivalent);
    }
  }
}

// Removes all events to be annihilated from |events|.  Given |intersection|,
// removes all events (both positive and negative) that are in the |events|
// array and the intersection.
void DoAnnihilation(std::vector<Event> const & intersection,
                    std::vector<Event>* events) {

  // For each event in the intersection, find the corresponding positive
  // and negative event in |events|, and remove them.
  // TODO:  Remove this O(N^2) logic.
  std::vector<Event>::const_iterator annihil_iter(intersection.begin()),
    annihil_end(intersection.end());
  for (; annihil_iter != annihil_end; ++annihil_iter) {
    assert(annihil_iter->is_positive());
    // Erase both the positive and negative version of the event.
    std::vector<Event>::iterator found_pos =
        std::find(events->begin(), events->end(), *annihil_iter);
    assert(found_pos != events->end());
    events->erase(found_pos);

    Event negative_event = *annihil_iter;
    negative_event.negate();
    std::vector<Event>::iterator found_neg =
        std::find(events->begin(), events->end(), negative_event);
    assert(found_neg != events->end());
    events->erase(found_neg);
  }
}

}  // namespace


LogicalProcess::~LogicalProcess() {
  // Release all of the resources allocated by the states.
  std::vector<State*>::iterator iter(states_.begin()), end(states_.end());
  for (; iter != end; ++iter) {
    delete *iter;
  }
  states_.clear();
}

std::ostream& LogicalProcess::log() const {
  return logger_->log() << name() << "@" << LogicalTime() <<" : ";
}

void LogicalProcess::ReceiveEvent(Event const & event) {
  input_queue_.push_back(event);
}

void LogicalProcess::ResolveInputQueue(std::vector<Event>* input_queue) {
  // First pull apart the postive and negative events.
  std::vector<Event> positive_events, negative_events;
  std::vector<Event>::iterator input_iter(input_queue->begin()),
      input_end(input_queue->end());
  for (; input_iter != input_end; ++input_iter) {
    if (input_iter->is_positive()) {
      positive_events.push_back(*input_iter);
    } else {
      negative_events.push_back(*input_iter);
    }
  }

  // Compute the set of matching positive/negative event pairs.
  std::vector<Event>  intersection;
  PositiveNegativeIntersect(positive_events, negative_events,
                            &intersection);

  std::vector<Event>::iterator annihil_iter(intersection.begin()),
    annihil_end(intersection.end());
  for (; annihil_iter != annihil_end; ++annihil_iter) {
    // Log notification of annihilation.
    log() << "Annihilating Event: " << annihil_iter->receive_time_stamp() <<
        " to: " << annihil_iter->target_process_id() << 
        " data: " << annihil_iter->payload() << " type: " <<
        annihil_iter->type();
  }

  // Remove all events corresponding to matching positive /negative pairs.
  DoAnnihilation(intersection, input_queue);
}

void LogicalProcess::EvaluateInputQueue(
    ProcessEnvironment* process_environment) {

  std::ostream& log_stream = log();
  log_stream << "Evaluating Input Queue: [";
  for (size_t x = 0; x < input_queue_.size(); ++x) {
    log_stream << "{ time: " << input_queue_[x].receive_time_stamp() <<
        " payload: " << input_queue_[x].payload() <<
        " type: " << input_queue_[x].type() << " }, ";
  }
  log_stream << "]";
  // Perform anti-message/message annihilation.
  ResolveInputQueue(&input_queue_);

  // Evaluate all events.
  std::vector<Event> events_postponed;
  std::vector<Event>::iterator input_iter(input_queue_.begin()),
      input_end(input_queue_.end());
  for (; input_iter != input_end; ++input_iter) {
    // If an event was not processed, keep it in the input queue for
    // subsequent evaluation.
    if (!ProcessEvent(*input_iter, process_environment)) {
      events_postponed.push_back(*input_iter);
    }
  }

  input_queue_ = events_postponed;
}

bool LogicalProcess::ProcessEvent(
    Event const & event,
    ProcessEnvironment* process_environment) {
  bool event_consumed = false;
  log() << "Processing event: " << event.receive_time_stamp() <<
        " to: " << event.target_process_id() << 
        " data: " << event.payload() << " type: " << event.type();
  // This is a future event - assume arbitrary/safe processing of
  // same-timestamp events
  if (event.receive_time_stamp() >= LogicalTime()) {
    event_consumed = ProcessFutureEvent(event, process_environment);
  } else {  // event is a straggler anti-message, or message
    event_consumed = ProcessHistoricalEvent(event, process_environment);
  }

  return event_consumed;
}

bool LogicalProcess::ProcessFutureEvent(
    Event const & event,
    ProcessEnvironment* process_environment) {
  assert(event.receive_time_stamp() >= LogicalTime());
  bool event_consumed = false;

  // Process the event as valid.
  if (event.is_positive()) {
    PushState(BuildMemento());
    locally_sent_events_.clear();
    local_time_ = Evaluate(event, process_environment);
    PostProcessEvent(event);
    event_consumed = true;
  } else {  // event.is_negative()
    // We've received an anti-message from the future!
    // Positive message is in-bound
    assert(false);
  }
  return event_consumed;
}

bool LogicalProcess::ProcessHistoricalEvent(
    Event const & event,
    ProcessEnvironment* process_environment) {
  // If the message is an anti-event, for which no corresponding event
  // has been processed, then we have a late/straggler message - leave
  // it in the queue.
  if (event.is_negative()) {
    Event positive_version = event;
    positive_version.negate();

    std::vector<Event>::iterator found_event =
      std::find(processed_events_.begin(), processed_events_.end(),
                positive_version);
    if (found_event == processed_events_.end())
      return false;
  }

  // Roll the LP back in time, and capture the set of events that need
  // re-processing.
  std::vector<Event> coast_forward_events;
  Rollback(event.receive_time_stamp(), &coast_forward_events,
           process_environment);

  if (event.is_positive()) {
    // Process a straggler event.
    ProcessEvent(event, process_environment);
  } else { // event.is_negative()
    // Remove the annihilated event from the coast-forward set
    Event positive_version = event;
    positive_version.negate();
    assert(positive_version.is_positive());

    std::vector<Event>::iterator found =
        std::find(coast_forward_events.begin(), coast_forward_events.end(),
                  positive_version);
    assert(coast_forward_events.end() != found);
    coast_forward_events.erase(found);
  }

  // Re-post the coast-forward events to the environment.
  // TODO:  This sends the events right back into the queue.  They could be directly
  // evaluated here.
  std::vector<Event>::iterator coast_iter(coast_forward_events.begin()),
      coast_end(coast_forward_events.end());
  for (; coast_iter != coast_end; ++coast_iter) {
    process_environment->Send(*coast_iter);
  }

  return true;
}

void LogicalProcess::PostProcessEvent(Event const& event) {
  // |event| has been evaluated, so finalize the structures required to roll
  // the event back.
  processed_events_.push_back(event);
  sent_events_.push_back(locally_sent_events_);
  locally_sent_events_.clear();
}

void LogicalProcess::SendEvent(Event const& event,
                               ProcessEnvironment* process_environment) {
  // Negate this event in the output queue
  locally_sent_events_.push_back(event);
  locally_sent_events_.back().negate();

  log() << "Sending Event: @" << event.receive_time_stamp() <<
        " to: " << event.target_process_id() << 
        " data: " << event.payload() << " type: " << event.type();

  process_environment->Send(event);
}

void LogicalProcess::Rollback(Time time,
                              std::vector<Event>* to_re_evaluate,
                              ProcessEnvironment* process_environment) {
  assert(states_.size() == processed_events_.size() &&
         sent_events_.size() == states_.size());

  log() << "Rolling back to time: " << time;

  // Roll back through the previously processed states, until the last state
  // evaluated before |time| is found.
  State* previous_state = NULL;
  while (!processed_events_.empty()) {
    Event& most_recent = processed_events_.back();
    if (most_recent.receive_time_stamp() > time) {
      delete previous_state;
      previous_state = states_.back();
      states_.pop_back();

      std::vector<Event>::iterator
          anti_message_iter(sent_events_.back().begin()),
          anti_message_end(sent_events_.back().end());
      for (; anti_message_iter != anti_message_end; ++anti_message_iter) {
        log() << "Sending Anti-Event: " << 
            anti_message_iter->receive_time_stamp() <<
            " to: " << anti_message_iter->target_process_id() << 
            " data: " << anti_message_iter->payload() << 
            " type: " << anti_message_iter->type();
        process_environment->Send(*anti_message_iter);
      }
      sent_events_.pop_back();

      to_re_evaluate->push_back(most_recent);
      processed_events_.pop_back();
    } else {
      break;
    }
  }

  // Restore the state of the LP appropriately.
  if (previous_state) {
    ResurrectMemento(previous_state);
    delete previous_state;
  }

  log() << "Rolled to: " << LogicalTime();
}

Time LogicalProcess::LocalVirtualTime() const {
  Time min_time= local_time_;
  // Return the minimum value of all events presently stored in the
  // input queue.
  // Note:  Under normal operation the input queue will be empty.  Input queue
  // resolution will only leave unmatched stragglers in the input-queue.
  std::vector<Event>::const_iterator input_iter(input_queue_.begin()),
      input_end(input_queue_.end());
  for (; input_iter != input_end; ++input_iter) {
    if (input_iter->receive_time_stamp() < min_time) {
      min_time = input_iter->receive_time_stamp();
    }
  }

  log() << "Computed virtual time: " << min_time;

  return min_time;
}

void LogicalProcess::FossilCollect(Time gvt) {
  assert(states_.size() == processed_events_.size() &&
         sent_events_.size() == states_.size());

  log() << "GVT Received: " << gvt;

  // Clear all states, processed events, and sent events
  size_t offset = 0;
  while (offset < processed_events_.size() &&
         processed_events_[offset].receive_time_stamp() < gvt) {
    delete states_[offset];
    ++offset;
  }

  processed_events_.erase(processed_events_.begin(),
                          processed_events_.begin() + offset);
  sent_events_.erase(sent_events_.begin(),
                     sent_events_.begin() + offset);
  states_.erase(states_.begin(), states_.begin() + offset);

  // Make sure that the memory was actually reclaimed.
  processed_events_.shrink_to_fit();
  sent_events_.shrink_to_fit();
  states_.shrink_to_fit();

  log() << "Fossil Collected: " << offset << " events.";
}
