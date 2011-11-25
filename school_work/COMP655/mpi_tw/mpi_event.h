#ifndef INCLUDED_MPI_EVENT_H_
#define INCLUDED_MPI_EVENT_H_

#include "event.h"

#include "mpi.h"

#include <assert.h>
#include <iostream>

class MPIEventRouter {
 public:
  // The set of MPI tags used by the system.
  enum {
    MPI_EVENT_TAG = 0x42,         // Sent event tag.
    MPI_EVENT_ACK_TAG,            // Event ack tag.
    MPI_GVT_REQUEST_TAG,          // GVT global request tag.
    MPI_GVT_LOCAL_RESPONSE_TAG,   // Individual GVT response tag.
    MPI_GVT_GLOBAL_RESPONSE_TAG   // GVT global response tag.
  };


  MPIEventRouter() {
    Init();
  }
  ~MPIEventRouter() {
    MPI_Type_free(&mpi_event_type_);
  }

  // Send |event| to MPI node |rank|.
  void MPISendEvent(int rank, Event const& event) {
    Event *non_const = const_cast<Event*>(&event);
    MPI_Send(reinterpret_cast<void*>(non_const), 1, mpi_event_type_, rank,
             MPI_EVENT_TAG, MPI_COMM_WORLD);
  }

  // Receive an |event| from a node.  |rank| is the rank from which
  // the event was received.
  // Returns true if an event was received.
  bool MPIReceiveEvent(Event* event, int* rank) {
    MPI_Status status;

    int pending_receive;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_EVENT_TAG, MPI_COMM_WORLD,
               &pending_receive, &status);
    if (pending_receive) {
      MPI_Recv(event, 1, mpi_event_type_, MPI_ANY_SOURCE, MPI_EVENT_TAG,
               MPI_COMM_WORLD, &status);
      *rank = status.MPI_SOURCE;
    }

    return !!pending_receive;
  }

  // Send an ack |event| to MPI node |rank|.
  void MPISendEventAck(int rank, Event const& event) {
    Event *non_const = const_cast<Event*>(&event);
    int rc = MPI_Send(reinterpret_cast<void*>(non_const), 1, mpi_event_type_, rank,
                      MPI_EVENT_ACK_TAG, MPI_COMM_WORLD);
  }

  // Receive an ack |event|.  |rank| is the rank of the node that sent the ack.
  // Returns true if an ack was received.
  bool MPIReceiveEventAck(Event* event, int* rank) {
    MPI_Status status;

    int pending_receive;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_EVENT_ACK_TAG, MPI_COMM_WORLD,
               &pending_receive, &status);
    if (pending_receive) {
      MPI_Recv(event, 1, mpi_event_type_, MPI_ANY_SOURCE, MPI_EVENT_ACK_TAG,
               MPI_COMM_WORLD, &status);
      *rank = status.MPI_SOURCE;
    }

    return !!pending_receive;
  }

 private:

  void Init() {
#if 0
    Time send_time_stamp_;
    Time receive_time_stamp_;
    int payload_;
    int type_;
    int sending_process_id_;
    int target_process_id_;
    bool marked_;
#endif

    // Construct a custom data-type format for sending Event instances via
    // MPI.
    MPI_Datatype member_types[] = {MPI_UNSIGNED_LONG, MPI_INT, MPI_CHAR};
    int member_blocks[] = {2, 4, 1}; 
    MPI_Aint member_offsets[3];
    MPI_Aint extent;

    member_offsets[0] = 0;
    MPI_Type_extent(member_types[0], &extent);
    member_offsets[1] = member_blocks[0] * extent;
    MPI_Type_extent(member_types[1], &extent);
    member_offsets[2] = member_offsets[1] + member_blocks[1] * extent;

    int rc = MPI_SUCCESS;
    rc = MPI_Type_struct(3, member_blocks, member_offsets, member_types,
                         &mpi_event_type_);
    assert(MPI_SUCCESS == rc);

    rc = MPI_Type_commit(&mpi_event_type_);
    assert(MPI_SUCCESS == rc);
  }

  MPI_Datatype mpi_event_type_;
};

// Global helper routines for sending GVT request/response messages.
inline void MPISendGVTRequest(int rank) {
  char value = 42;
  MPI_Send(&value, 1, MPI_CHAR, rank, MPIEventRouter::MPI_GVT_REQUEST_TAG,
           MPI_COMM_WORLD);
}

// Receive a GVT request from the controller.  |controller_rank| is assigned
// the rank of the controller.
// Returns true if a request was received.
inline bool MPIReceiveGVTRequest(int* controller_rank) {
  MPI_Status status;
  int pending_receive;
  MPI_Iprobe(MPI_ANY_SOURCE, MPIEventRouter::MPI_GVT_REQUEST_TAG, MPI_COMM_WORLD,
              &pending_receive, &status);
  if (pending_receive) {
    char value;
    MPI_Recv(&value, 1, MPI_CHAR, MPI_ANY_SOURCE,
              MPIEventRouter::MPI_GVT_REQUEST_TAG, MPI_COMM_WORLD, &status);
    assert(42 == value);
    *controller_rank = status.MPI_SOURCE;
  }

  return !!pending_receive;
}

// Return a locally computed GVT value, |gvt|, to controller with rank
// |controller_rank|.
inline void MPISendLocalGVTResponse(Time* gvt, int controller_rank) {
  MPI_Send(gvt, 1, MPI_UNSIGNED_LONG, controller_rank,
           MPIEventRouter::MPI_GVT_LOCAL_RESPONSE_TAG, MPI_COMM_WORLD);
}

// Receive |gvt| response from an individual node with rank, |node_rank|.
// Returns fals if no node reported a response.
inline bool MPIReceiveLocalGVTResponse(Time* gvt, int* node_rank) {
  MPI_Status status;
  int pending_receive;
  MPI_Iprobe(MPI_ANY_SOURCE, MPIEventRouter::MPI_GVT_LOCAL_RESPONSE_TAG,
             MPI_COMM_WORLD, &pending_receive, &status);
  if (pending_receive) {
    MPI_Recv(gvt, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE,
             MPIEventRouter::MPI_GVT_LOCAL_RESPONSE_TAG, MPI_COMM_WORLD,
             &status);
    *node_rank = status.MPI_SOURCE;
  }

  return !!pending_receive;
}

// Send computed |gvt| value to node with |rank|.
inline void MPISendGlobalGVTResponse(Time* gvt, int rank) {
  MPI_Send(gvt, 1, MPI_UNSIGNED_LONG, rank,
           MPIEventRouter::MPI_GVT_GLOBAL_RESPONSE_TAG, MPI_COMM_WORLD);
}

// Receive a computed, global |gvt| value from the controller with rank,
// |controller_rank|.
// Returns true if a GVT value was received from the controller.
inline bool MPIReceiveGVTResponse(Time* gvt, int controller_rank) {
  // Probe for an event from the controller
  MPI_Status status;
  int pending_receive;
  MPI_Iprobe(controller_rank, MPIEventRouter::MPI_GVT_GLOBAL_RESPONSE_TAG,
             MPI_COMM_WORLD, &pending_receive, &status);
  if (pending_receive) {
    MPI_Recv(gvt, 1, MPI_UNSIGNED_LONG, controller_rank,
             MPIEventRouter::MPI_GVT_GLOBAL_RESPONSE_TAG, MPI_COMM_WORLD,
             &status);
  }

  return !!pending_receive;
}

#endif  // INCLUDED_MPI_EVENT_H_
