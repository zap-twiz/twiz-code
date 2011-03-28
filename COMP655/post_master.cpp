#include "stdafx.h"
#include "post_master.h"

#include "simulation_engine.h"

#include <assert.h>

void PartitionedPostMaster::SendMessage(Event const & event) {
    assert(engine_map_.find(event.target_process_id()) != engine_map_.end());
    ProcessEnvironment* env =
        engine_map_[event.target_process_id()]->environment();
    env->event_queue().RegisterEvent(event);
}
