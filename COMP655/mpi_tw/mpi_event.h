#ifndef INCLUDED_MPI_EVENT_H_
#define INCLUDED_MPI_EVENT_H_

#include "event.h"

#include "mpi.h"

#include <assert.h>
#include <iostream>

class MPIEventRouter {
 public:
  MPIEventRouter() {
    Init();
  }
  ~MPIEventRouter() {
    MPI_Type_free(&mpi_event_type_);
  }

  void MPISendEvent(int rank, Event const& event) {
    Event *non_const = const_cast<Event*>(&event);
    MPI_Send(reinterpret_cast<void*>(non_const), 1, mpi_event_type_, rank,
             MPI_EVENT_TAG, MPI_COMM_WORLD);
  }

  void MPIReceiveEvent(Event* event) {
    MPI_Status status;
    MPI_Recv(event, 1, mpi_event_type_, 0, MPI_EVENT_TAG,
             MPI_COMM_WORLD, &status);

#if 0
    std::cout << "MPI RC error:" << rc << std::endl;
    std::cout << "Received-status.count:" << status.count <<
        ", cancelled: " << status.cancelled << ", Error" << status.MPI_ERROR << std::endl;
#endif
  }

  void MPIStartGVT( ) {
  }

  void MPIReportGVT() {
  }

 private:
  enum {
    MPI_EVENT_TAG = 0x1000,
    MPI_EVENT_ACK_TAG,
    MPI_GVT_REQUEST_TAG,
    MPI_GVT_RESPONSE_TAG
  };
  void Init() {
#if 0
    Time send_time_stamp_;
    Time receive_time_stamp_;
    int payload_;
    int type_;
    int sending_process_id_;
    int target_process_id_;
#endif

    /*bool find_mode_;*/
    MPI_Datatype member_types[] = {MPI_UNSIGNED_LONG, MPI_INT};
    int member_blocks[] = {2, 4}; 
    MPI_Aint member_offsets[2];
    MPI_Aint extent;

    member_offsets[0] = 0;
    MPI_Type_extent(member_types[0], &extent);
    member_offsets[1] = member_blocks[0] * extent;

    int rc = MPI_SUCCESS;
    rc = MPI_Type_struct(2, member_blocks, member_offsets, member_types,
                         &mpi_event_type_);
    assert(MPI_SUCCESS == rc);

    rc = MPI_Type_commit(&mpi_event_type_);
    assert(MPI_SUCCESS == rc);
  }

  MPI_Datatype mpi_event_type_;
};

#endif  // INCLUDED_MPI_EVENT_H_
