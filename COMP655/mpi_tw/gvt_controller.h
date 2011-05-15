#ifndef INCLUDED_GVT_CONTROLLER_H_
#define INCLUDED_GVT_CONTROLLER_H_

#include "mpi_event.h"

#include <assert.h>

#include "mpi.h"

class GVTController {
 public:
  GVTController(int num_nodes, int rank)
    : responses_received_(0),
      num_nodes_(num_nodes),
      rank_(rank) {}

  void BroadcastGVTStart() {
    assert(0 == responses_received_);
    // Request that ALL compute nodes perform a GVT computation.
    for (int x = 0; x < num_nodes_; ++x) {
      if (x != rank_)
        MPISendGVTRequest(x);
     }
  };

  bool ReceiveGVTResponse() {
    Time gvt;
    int node;
    if (MPIReceiveLocalGVTResponse(&gvt, &node)) {
      if (0 == responses_received_)
        min_gvt_ = gvt;

      if (gvt < min_gvt_)
        min_gvt_ = gvt;
      ++responses_received_;
    }
    return responses_received_ == num_nodes_ - 1;
  }; 

  void BroadcastGVTValue() {
    assert(num_nodes_ - 1 == responses_received_);
    for (int x = 0; x < num_nodes_; ++x) {
      if (x != rank_)
        MPISendGlobalGVTResponse(&min_gvt_, x);
    }

    // Reset for the next round
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
