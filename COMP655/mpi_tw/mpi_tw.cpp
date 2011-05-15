// mpi_tutorial.cpp : Defines the entry point for the console application.
//


#include "gvt_controller.h"
#include "mpi_event.h"
#include "mpi_post_master.h"
#include "mpi_topology_builder.h"

#include <assert.h>
#include <iostream>

#include "mpi.h"
#include <Windows.h>


int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  while(true) {}

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  assert(3 == size);

  if (rank == 2 ) {
    GVTController gvt_controller(size, rank);
    while (true) {
      gvt_controller.BroadcastGVTStart();
      while (!gvt_controller.ReceiveGVTResponse()) {
      }
      gvt_controller.BroadcastGVTValue();
      Sleep(500);
    }
  }

  if (rank != 2 ) {
    SimulationEngine engine;
    MPIPostMaster post_master;
    MPITopologyBuilder mpi_builder(rank, &post_master);
    engine.Init(&mpi_builder);

    if (0 == rank) {
      while(true) {
        engine.TimeStep();
        std::cout << "Master Engine @ " << engine.LocalVirtualTime() <<
            std::endl;
      }
#if 0
      for (int x = 0; x < 6000; ++x) {
        engine.TimeStep();
        std::cout << "Master Engine @ " << engine.LocalVirtualTime() <<
            std::endl;
      }
#endif
    } else if (1 == rank) {
      while(true) {
        engine.TimeStep();
        std::cout << "Slave Engine @ " << engine.LocalVirtualTime() <<
            std::endl;

      }
#if 0
      for (int x = 0; x < 6000; ++x) {
        engine.TimeStep();
        std::cout << "Slave Engine @ " << engine.LocalVirtualTime() <<
            std::endl;
      }
#endif
    }
  }  // block scope
  /* do nothing */
  MPI_Finalize();
  return 0;
}

#if 0
  if (0 == rank) {
    std::cout << "Master Reporting" << std::endl;
    MPIPostMaster post_master;
    MPIEventRouter  event_router;

    Event test_event;
    test_event.set_receive_time_stamp(0xA);
    test_event.set_send_time_stamp(0xB);
    test_event.set_sending_process_id(0xC);
    test_event.set_payload(0xD);
    test_event.set_type(0xE);
    test_event.set_target_process_id(0xF);
    event_router.MPISendEvent(1, test_event);
  } else if (1 == rank) {
    std::cout << "Slave Reporting" << std::endl;

    Event receive_event;
    MPIEventRouter event_router;
    event_router.MPIReceiveEvent(&receive_event);

    std::cout << "Slave Received: " <<
      "Recv@ " << receive_event.receive_time_stamp() << ", " <<
      "Send@ " << receive_event.send_time_stamp() << ", " <<
      "Pay@ " << receive_event.payload() << ", " <<
      "Type@ " << receive_event.type() << std::endl;
  }
#endif

#if 0
#define WORK_TAG      1
#define WORK_CONFIRM  2
#define WORK_RESULT   3

#define DIE_TAG   100

int fibonacci(int n) {
  if (1 == n)
    return 1;
  if (0 == n)
    return 1;

  return fibonacci(n-1) + fibonacci(n-2);
}

void master_loop(int rank, int size) {

  int task_value = 0;

  for (int i = 1; i < size; ++i) {
    //work = get_next_work_item();
    /* Send it to each rank */
    MPI_Send(&task_value,             /* message buffer */
              1,                 /* one data item */
              MPI_INT,           /* data item is an integer */
              i,              /* destination process rank */
              WORK_TAG,               /* user chosen message tag */
              MPI_COMM_WORLD);   /* default communicator */
    std::cout << "Master sent request " << task_value << " to proc " << i << std::endl;
    ++task_value;
  }

  int rounds = 200000;
  MPI_Status status;
  while (--rounds > 0) {
    int result;
    MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, WORK_RESULT, MPI_COMM_WORLD, &status);
    std::cout << "Proc: " << status.MPI_SOURCE << " computed: " << result << std::endl;
    MPI_Send(&task_value, 1, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
    ++task_value;
    task_value = task_value % 150;
  }

  // Receive Results
  for (int i = 1; i < size; ++i) {
    int result;
    MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE,
             MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    std::cout << "Proc: " << status.MPI_SOURCE << " computed: " << result << std::endl;
  }

  // Send Kill Message
  for (int i = 1; i < size; ++i) {
    MPI_Send(0, 0, MPI_INT, i, DIE_TAG, MPI_COMM_WORLD);
  }
};

void slave_loop(int rank, int size) {
  MPI_Status status;
  while (1) {
    int work;
    MPI_Recv(&work, 1, MPI_INT, 0, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);


    if (DIE_TAG == status.MPI_TAG) {
      std::cout << "Slave " << rank << " received death request" << std::endl;
      return;
    }

    std::cout << "Slave " << rank << "received work request" << work << std::endl;

    int result = fibonacci(work);
    MPI_Send(&result, 1, MPI_INT, 0, WORK_RESULT, MPI_COMM_WORLD);
  }
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  //std::cout << "hello?" << std::endl;
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::cout << "Startup:  Proc " << rank << " of " << size << std::endl;

  if (0 == rank) {
    master_loop(rank, size);
  } else {
    slave_loop(rank, size);
  }

  MPI_Finalize();
  return 0;
} 

#endif 