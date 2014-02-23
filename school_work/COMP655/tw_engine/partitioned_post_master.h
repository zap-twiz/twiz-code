#ifndef INCLUDED_PARTITIONED_POST_MASTER_H__
#define INCLUDED_PARTITIONED_POST_MASTER_H__

#include "post_master.h"
#include "process_environment.h"

#include <map>


class SimulationEngine;

class PartitionedPostMaster : public PostMaster {
 public:
  void RegisterRemoteLP(int lp_id, SimulationEngine* engine) {
    engine_map_[lp_id] = engine;
  }

  virtual void ResolveAckMessages() {}

  virtual bool LocalVirtualTimeContribution(Time* time) const {
    // TODO:  Process messages appropriately!
    return false;
  }

  virtual void SendMessage(Event const & event);
  virtual bool ReceiveMessage(Event* event) {
    return false;
  }

 protected:
  SimulationEngine* engine(int lp_id);
 private:
  std::map<int, SimulationEngine*> engine_map_;
};

#endif  // INCLUDED_PARTITIONED_POST_MASTER_H__
