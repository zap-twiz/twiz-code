#ifndef INCLUDED_POST_MASTER_H_
#define INCLUDED_POST_MASTER_H_

#include "event.h"

// The |PostMaster| interface provides an abstraction for all inter-simulation
// message sends.  All traffic from within a |ProcessEnvironment| will be routed
// to a post-master instance.
// Entry points are also provided for management of GVT-related message
// processing.
class PostMaster {
 public:
  virtual ~PostMaster() {}

  // Interface used to send messages.  Should be implemented to send to
  // separate SimulationEngines in the same process, or separate processes
  // via MPI.
  virtual void SendMessage(Event const & event) = 0;

  // Returns true if there is an event to receive.  |event| will contain the
  // data of the event.
  virtual bool ReceiveMessage(Event* event) = 0;

  // Called to receive and manage message acknowledgement.
  virtual void ResolveAckMessages() = 0;

  // The post master stores the message-acknowledge ment, and marked-message
  // structures.  This interface routine is invoked to provide a potential
  // gvt value to the system.
  virtual bool LocalVirtualTimeContribution(Time* time) const {
    // The default behaviour is that the post-master has no
    // contribution to the GVT computations.
    return false;
  }

  // Interface point for receiving notification of a GVT request.
  // Returns true if a request has been received.  Default is no GVT
  // integration.
  virtual bool ReceiveGVTRequest() { return false; }

  // Interface point that receives the gvt value from the GVT controller
  // returns true if a GVT has been received.
  virtual bool ReceiveGVTValue(Time* gvt) { return false; }

  // Interface for posting the locally computed GVT time to the controller.
  virtual void SendGVTResponse(Time gvt) {}

  // The post-master tracks the current find-mode setting of the
  // simulation.
  void set_find_mode(bool mode) { find_mode_ = mode; }
  bool find_mode() const { return find_mode_; }

 private:
  bool find_mode_;
};

#endif  // INCLUDED_POST_MASTER_H_