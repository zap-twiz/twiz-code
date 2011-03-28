// mpi_tutorial.cpp : Defines the entry point for the console application.
//

#include "mpi_post_master.h"
#include "mpi_topology_builder.h"

#include <iostream>
#include "mpi.h"

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPIPostMaster post_master;

  MPI_Finalize();
  return 0;
}
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