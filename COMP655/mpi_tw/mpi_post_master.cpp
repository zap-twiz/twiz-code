
#include "mpi_post_master.h"
#include "simulation_engine.h"

void MPIPostMaster::SendMessage(Event const & event) {
  SimulationEngine* local_engine = engine(event.target_process_id());
  if (local_engine) {
    ProcessEnvironment* env = local_engine->environment();
    Event local_copy = event;
    local_copy.set_find_mode(find_mode());
    env->Send(local_copy);
  } else {
    /* send a message via MPI */
  }
}
