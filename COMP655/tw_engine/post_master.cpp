#include "stdafx.h"
#include "post_master.h"

#include "simulation_engine.h"

#include <assert.h>

void PartitionedPostMaster::SendMessage(Event const & event) {
    assert(engine_map_.find(event.target_process_id()) != engine_map_.end());

    // TODO:  Clean up the find-mode usage!
    ProcessEnvironment* env =
        engine_map_[event.target_process_id()]->environment();
    Event local_copy = event;
    local_copy.set_find_mode(find_mode());
    env->event_queue().RegisterEvent(local_copy);
}

SimulationEngine* PartitionedPostMaster::engine(int lp_id) {
  std::map<int, SimulationEngine*>::iterator found = engine_map_.find(lp_id);
  if (found == engine_map_.end()) {
    return NULL;
  } else {
    return found->second;
  }
}
