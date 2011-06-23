#include "stdafx.h"
#include "partitioned_post_master.h"

#include "simulation_engine.h"

#include <assert.h>

void PartitionedPostMaster::SendMessage(Event const & event) {
    assert(engine_map_.find(event.target_process_id()) != engine_map_.end());

    // Register the event with the input queue of the |ProcessEnvironment| hosted
    // in the appropriate engine.
    ProcessEnvironment* env =
        engine_map_[event.target_process_id()]->environment();
    env->event_queue().RegisterEvent(event);
}

SimulationEngine* PartitionedPostMaster::engine(int lp_id) {
  std::map<int, SimulationEngine*>::iterator found = engine_map_.find(lp_id);
  if (found == engine_map_.end()) {
    return NULL;
  } else {
    return found->second;
  }
}
