
#include "mpi_post_master.h"
#include "mpi_event.h"
#include "simulation_engine.h"

#include <assert.h>
#include "mpi.h"

void MPIPostMaster::SendMessage(Event const & event) {
  SimulationEngine* local_engine = engine(event.target_process_id());
  if (local_engine) {
    ProcessEnvironment* env = local_engine->environment();
    Event local_copy = event;
    //local_copy.set_find_mode(find_mode());
    env->event_queue().RegisterEvent(local_copy);
  } else {
    /* send a message via MPI */
    std::map<int, int>::iterator found_remote_rank_ = 
        lp_rank_map_.find(event.target_process_id());
    assert(lp_rank_map_.end() != found_remote_rank_);
    event_router_.MPISendEvent(found_remote_rank_->second, event);
  }
}

bool MPIPostMaster::ReceiveMessage(Event* event) {
  event_router_.MPIReceiveEvent(event);
  return true;
}
