#ifndef INCLUDED_POST_MASTER_H_
#define INCLUDED_POST_MASTER_H_

class Event;

class PostMaster {
 public:
  virtual ~PostMaster() {}
  virtual void SendMessage(Event const & event) = 0;

  //virtual bool IsLPLocal(int lp_id) const = 0;
};

class LocalPostMaster : public PostMaster {
 public:
  virtual void SendMessage(Event const & event) {}
  //bool IsLPLocal(int) const { return true; }
};

#include <map>

//#include "simulation_engine.h"
#include "process_environment.h"

class SimulationEngine;

class PartitionedPostMaster : public PostMaster {
 public:
  void RegisterRemoteLP(int lp_id, SimulationEngine* engine) {
    engine_map_[lp_id] = engine;
  }

  virtual void SendMessage(Event const & event);
 private:
  std::map<int, SimulationEngine*> engine_map_;
};

#endif  // INCLUDED_POST_MASTER_H_