
#include "mpi_post_master.h"
#include "mpi_event.h"
#include "simulation_engine.h"

#include <algorithm>
#include <assert.h>
#include "mpi.h"

void MPIPostMaster::SendMessage(Event const & event) {
  SimulationEngine* local_engine = engine(event.target_process_id());
  // If the event is destined to an LP hosted by a simulation engine in the same
  // process, then directly enqueue it.
  if (local_engine) {
    ProcessEnvironment* env = local_engine->environment();
    Event local_copy = event;
    //local_copy.set_find_mode(find_mode());
    env->event_queue().RegisterEvent(local_copy);
  } else {
    // Send a message via MPI
    std::map<int, int>::iterator found_remote_rank_ = 
        lp_rank_map_.find(event.target_process_id());
    assert(lp_rank_map_.end() != found_remote_rank_);

    events_pending_ack_.push_back(event);
    event_router_.MPISendEvent(found_remote_rank_->second, event);
  }
}

bool MPIPostMaster::ReceiveMessage(Event* event) {
  int rank;
  if (event_router_.MPIReceiveEvent(event, &rank)) {
    // Send the corresponding ACK, based on the find status of GVT.
    if (find_mode()) {
      event->set_marked(true);
    } else {
      event->set_marked(false);
    }
    event_router_.MPISendEventAck(rank, *event);
    return true;
  }
  return false;
}

void MPIPostMaster::ResolveAckMessages() {
  Event event;
  int rank;
  while (event_router_.MPIReceiveEventAck(&event, &rank)) {
    // Update the marked time stamp if the ack was marked.
    if (event.is_marked()) {
      if (event.receive_time_stamp() < marked_event_time_)
        marked_event_time_ = event.receive_time_stamp();
    }

    // Receive the ACK, and remove it from the pending-ack queue
    std::vector<Event>::iterator found = std::find(
        events_pending_ack_.begin(), events_pending_ack_.end(), event);
    assert(found != events_pending_ack_.end());
    events_pending_ack_.erase(found);
  }
}

bool MPIPostMaster::ReceiveGVTRequest() {
  if (MPIReceiveGVTRequest(&gvt_controller_rank_)) {
    gvt_request_received_ = true;
    return true;
  }
  return false;
}

void MPIPostMaster::SendGVTResponse(Time gvt) {
  // We have sent our GVT response, so all subsequent acks will
  // be marked until we receive a GVT value.
  set_find_mode(true);
  MPISendLocalGVTResponse(&gvt, gvt_controller_rank_);
}

bool MPIPostMaster::ReceiveGVTValue(Time* gvt) {
  if (gvt_request_received_ && 
      MPIReceiveGVTResponse(gvt, gvt_controller_rank_)) {
    // Reset the find-mode statistics
    set_find_mode(false);
    marked_event_time_ = MAX_TIME;
    gvt_request_received_ = false;
    return true;
  }
  return false;
}
