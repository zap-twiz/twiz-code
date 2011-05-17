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
      // Invoke the GVT algorithm at 500 ms intervals.
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
    } else if (1 == rank) {
      while(true) {
        engine.TimeStep();
        std::cout << "Slave Engine @ " << engine.LocalVirtualTime() <<
            std::endl;
      }
    }
  }

  MPI_Finalize();
  return 0;
}
