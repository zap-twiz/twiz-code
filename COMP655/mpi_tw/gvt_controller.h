#ifndef INCLUDED_GVT_CONTROLLER_H_
#define INCLUDED_GVT_CONTROLLER_H_

#include "mpi_event.h"

#include <assert.h>

#include "mpi.h"

// The GVTController class lives in a separate MPI process, and coordinates
// the GVT computation among the different nodes.
class GVTController {
 public:
  // Initialize the controller with the |num_nodes| within the simulation,
  // and the MPI |rank| of the node 
  GVTController(int num_nodes, int rank)
    : responses_received_(0),
      num_nodes_(num_nodes),
      rank_(rank) {}

  // Send out a GVT request to all nodes in the system.
  void BroadcastGVTStart() {
    assert(0 == responses_received_);
    // Request that ALL compute nodes perform a GVT computation.
    for (int x = 0; x < num_nodes_; ++x) {
      if (x != rank_)
        MPISendGVTRequest(x);
     }
  };

  // Returns true if a gvt response has been received from every node.
  // TODO:  This routine's functionality should use the reduce functions
  // exported by MPI.
  bool ReceiveGVTResponse() {
    Time gvt;
    int node;
    if (MPIReceiveLocalGVTResponse(&gvt, &node)) {
      // Update the minimum of the GVT values received.
      if (0 == responses_received_)
        min_gvt_ = gvt;

      if (gvt < min_gvt_)
        min_gvt_ = gvt;
      ++responses_received_;
    }
    return responses_received_ == num_nodes_ - 1;
  }; 

  // Broadcast the computed GVT value to all nodes in the system.
  void BroadcastGVTValue() {
    assert(num_nodes_ - 1 == responses_received_);
    for (int x = 0; x < num_nodes_; ++x) {
      if (x != rank_)
        MPISendGlobalGVTResponse(&min_gvt_, x);
    }

    // Reset for the next round.
    responses_received_ = 0;
  }

 private:
  Time min_gvt_;
  int responses_received_;

  // Store status from the number of responses, etc.
  int num_nodes_;
  int rank_;
};

#endif
