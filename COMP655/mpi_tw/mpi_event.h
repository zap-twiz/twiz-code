#ifndef INCLUDED_MPI_EVENT_H_
#define INCLUDED_MPI_EVENT_H_

#include "event.h"

#include "mpi.h"

#include <assert.h>
#include <iostream>

class MPIEventRouter {
 public:
  enum {
    MPI_EVENT_TAG = 0x42,
    MPI_EVENT_ACK_TAG,
    MPI_GVT_REQUEST_TAG,
    MPI_GVT_LOCAL_RESPONSE_TAG,
    MPI_GVT_GLOBAL_RESPONSE_TAG
  };


  MPIEventRouter() {
    Init();
  }
  ~MPIEventRouter() {
    MPI_Type_free(&mpi_event_type_);
  }

  void MPISendEvent(int rank, Event const& event) {
    Event *non_const = const_cast<Event*>(&event);
//    MPIEvent mpi_event;
    MPI_Send(reinterpret_cast<void*>(non_const), 1, mpi_event_type_, rank,
             MPI_EVENT_TAG, MPI_COMM_WORLD);
  }

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

  void MPISendEventAck(int rank, Event const& event) {
    Event *non_const = const_cast<Event*>(&event);
    int rc = MPI_Send(reinterpret_cast<void*>(non_const), 1, mpi_event_type_, rank,
                      MPI_EVENT_ACK_TAG, MPI_COMM_WORLD);
  }

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

  void MPIStartGVT( ) {
  }

  void MPIReportGVT() {
  }

 private:
  struct MPIEvent {
    int mpi_message_type_;
    Time send_time_stamp_;
    Time receive_time_stamp_;
    int payload_;
    int type_;
    int sending_process_id_;
    int target_process_id_;
  };


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

    /*bool find_mode_;*/
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

inline void MPISendGVTRequest(int rank) {
  char value = 42;
  MPI_Send(&value, 1, MPI_CHAR, rank, MPIEventRouter::MPI_GVT_REQUEST_TAG,
           MPI_COMM_WORLD);
}

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

inline void MPISendLocalGVTResponse(Time* gvt, int controller_rank) {
  MPI_Send(gvt, 1, MPI_UNSIGNED_LONG, controller_rank,
           MPIEventRouter::MPI_GVT_LOCAL_RESPONSE_TAG, MPI_COMM_WORLD);
}

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

inline void MPISendGlobalGVTResponse(Time* gvt, int rank) {
  MPI_Send(gvt, 1, MPI_UNSIGNED_LONG, rank,
           MPIEventRouter::MPI_GVT_GLOBAL_RESPONSE_TAG, MPI_COMM_WORLD);
}

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
