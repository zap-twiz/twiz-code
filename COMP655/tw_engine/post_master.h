#ifndef INCLUDED_POST_MASTER_H_
#define INCLUDED_POST_MASTER_H_

#include "event.h"

class PostMaster {
 public:
  virtual ~PostMaster() {}
  virtual void SendMessage(Event const & event) = 0;
  virtual bool ReceiveMessage(Event* event) = 0;
  virtual void ResolveAckMessages() = 0;

  virtual bool LocalVirtualTimeContribution(Time* time) const {
    // The default behaviour is that the post-master has no
    // contribution to the GVT computations
    return false;
  }

  virtual bool ReceiveGVTRequest() { return false; }
  virtual bool ReceiveGVTValue(Time* gvt) { return false; }
  virtual void SendGVTResponse(Time gvt) {}

  // TODO:  Remove these!  (OR integrate them better)
  void set_find_mode(bool mode) { find_mode_ = mode; }
  bool find_mode() const { return find_mode_; }

 private:
  bool find_mode_;
};

#include <map>
#include "process_environment.h"

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

#endif  // INCLUDED_POST_MASTER_H_