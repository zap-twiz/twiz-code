// single_proc_tw.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <functional>
#include <iostream>

//using namespace std;

#include "simulation_engine.h"
#include "basic_topology_builder.h"
#include "post_master.h"

int _tmain(int argc, _TCHAR* argv[]) {
  std::vector<SimulationEngine> engines;
  engines.resize(2);

  PartitionedPostMaster post_master;
  PartitionedTopologyBuilder builder1(0, &engines, &post_master),
      builder2(1, &engines, &post_master);

  engines[0].Init(&builder1);
  engines[1].Init(&builder2);

  int iter_count = 0;
  while (!(engines[0].IsIdle() && engines[1].IsIdle())) {

    if (!(iter_count % 400)) {
      Time engine1 = engines[0].LocalVirtualTime();
      Time engine2 = engines[1].LocalVirtualTime();
      Time gvt = engine1 < engine2 ? engine1 : engine2;
      engines[0].ReceiveGlobalVirtualTime(gvt);
      engines[1].ReceiveGlobalVirtualTime(gvt);
    }
    engines[0].TimeStep();
    engines[0].TimeStep();
    engines[0].TimeStep();
    engines[1].TimeStep();
    std::cout << "Engine 1@: " << engines[0].LocalVirtualTime() <<
        ", Engine 2@: " << engines[1].LocalVirtualTime() << std::endl;
    ++iter_count;
  }


#if 0
  while (!(engines[0].IsIdle() && engines[1].IsIdle())) {
    engines[0].TimeStep();
    engines[1].TimeStep();
    std::cout << "Engine 1@: " << engines[0].LocalVirtualTime() <<
        ", Engine 2@: " << engines[1].LocalVirtualTime() << std::endl;
  }

  Event bogus_event;
  bogus_event.set_receive_time_stamp(100);
  bogus_event.set_send_time_stamp(100);
  bogus_event.set_sending_process_id(-1);
  bogus_event.set_payload(-1);
  bogus_event.set_type(47);
  bogus_event.set_target_process_id(builder1.generator1()->id());
  post_master.SendMessage(bogus_event);

  while (!(engines[0].IsIdle() && engines[1].IsIdle())) {
    engines[0].TimeStep();
    engines[1].TimeStep();
    std::cout << "Engine 1@: " << engines[0].LocalVirtualTime() <<
        ", Engine 2@: " << engines[1].LocalVirtualTime() << std::endl;
  }
#endif

#if 0
  int step_count = 0;
  while (step_count < 10) {
    engine.TimeStep();
    ++step_count;
#if 1
    if (step_count == 7) {
      Event bogus_event;
      bogus_event.set_receive_time_stamp(100);
      bogus_event.set_send_time_stamp(100);
      bogus_event.set_sending_process_id(-1);
      bogus_event.set_payload(-1);
      bogus_event.set_type(47);
      bogus_event.set_target_process_id(simple_builder.generator1()->id());
      engine.environment()->Send(bogus_event);
    }
#endif
  }

  //simple_builder.generator1()->Inhibit();
  //simple_builder.generator2()->Inhibit();

  while (!engine.IsIdle()) {
    engine.TimeStep();
  }

#if 1
  Event bogus_event;
  bogus_event.set_receive_time_stamp(0);
  bogus_event.set_send_time_stamp(0);
  bogus_event.set_sending_process_id(-1);
  bogus_event.set_payload(20000);
  bogus_event.set_type(666);
  bogus_event.set_target_process_id(simple_builder.generator2()->id());
  engine.environment()->Send(bogus_event);
#endif

  while (!engine.IsIdle()) {
    engine.TimeStep();
  }
#endif

  return 0;
}
