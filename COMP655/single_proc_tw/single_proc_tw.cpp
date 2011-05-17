// single_proc_tw.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <functional>
#include <iostream>

//using namespace std;

#include "simulation_engine.h"
#include "partitioned_builder.h"
#include "partitioned_post_master.h"

int _tmain(int argc, _TCHAR* argv[]) {
  std::vector<SimulationEngine> engines;
  engines.resize(2);

  // Construct two simulatoin engines, representing each half of the model.
  PartitionedPostMaster post_master;
  PartitionedTopologyBuilder builder1(0, &post_master),
      builder2(1, &post_master);
  engines[0].Init(&builder1);
  engines[1].Init(&builder2);

  int iter_count = 0;
  int step = 0;

  // Process until both engines are idle.
  while (!(engines[0].IsIdle() && engines[1].IsIdle())) {

    // Every 400 iterations, extract the virtual time, and push the GVT
    // to the respective simulation engines.
    if (!(iter_count % 400)) {
      Time engine1 = engines[0].LocalVirtualTime();
      Time engine2 = engines[1].LocalVirtualTime();
      Time gvt = engine1 < engine2 ? engine1 : engine2;
      engines[0].ReceiveGlobalVirtualTime(gvt);
      engines[1].ReceiveGlobalVirtualTime(gvt);

      // Be horrible, and send an event at gvt.
      Event bogus_event;
      bogus_event.set_receive_time_stamp(gvt);
      bogus_event.set_send_time_stamp(gvt);
      bogus_event.set_sending_process_id(666);
      bogus_event.set_payload(667);
      bogus_event.set_type(1337);
      bogus_event.set_target_process_id(builder1.generator1()->id());
      post_master.SendMessage(bogus_event);
    }

    // Time step the engines a differing number of times, so that
    // they become out of sync, and the buffers can fill up.
    int time_steps[] = {1, 3, 2, 5, 3, 7, 2};
    for (int x = 0; x < time_steps[step%7]; ++x )
      engines[0].TimeStep();
    ++step;

    for (int x = 0; x < time_steps[step%7]; ++x )
      engines[1].TimeStep();

    std::cout << "Engine 1@: " << engines[0].LocalVirtualTime() <<
        ", Engine 2@: " << engines[1].LocalVirtualTime() << std::endl;
    ++iter_count;
  }

  return 0;
}
