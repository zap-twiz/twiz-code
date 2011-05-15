#ifndef INCLUDED_MPI_POST_MASTER_H_
#define INCLUDED_MPI_POST_MASTER_H_

#include "post_master.h"
#include "mpi_event.h"

#include <map>
#include <vector>

class MPIPostMaster : public PartitionedPostMaster {
 public:
  MPIPostMaster()
      : gvt_request_received_(false),
        marked_event_time_(MAX_TIME) {}

  virtual ~MPIPostMaster() {}
  virtual void SendMessage(Event const & event);
  virtual bool ReceiveMessage(Event* event);

  virtual void ResolveAckMessages();

  virtual bool ReceiveGVTRequest();
  virtual void SendGVTResponse(Time gvt);
  virtual bool ReceiveGVTValue(Time* gvt);

  void RegisterLPRank(int lp_id, int external_rank) {
    lp_rank_map_[lp_id] = external_rank;
  }

  virtual bool LocalVirtualTimeContribution(Time* time) const {
    *time = MAX_TIME;
    // Return the time-stamp of the earlist non-acked event
    if (!events_pending_ack_.empty()) {
      *time = events_pending_ack_[0].receive_time_stamp();
      for (size_t x = 1; x < events_pending_ack_.size(); ++x) {
        if (*time < events_pending_ack_[x].receive_time_stamp())
          *time = events_pending_ack_[x].receive_time_stamp();
      }
    }

    if (marked_event_time_ < *time)
      *time = marked_event_time_;

    return true;
  }

 private:
  std::map<int, int>  lp_rank_map_;

  std::vector<Event> events_pending_ack_;
  MPIEventRouter event_router_;

  int gvt_controller_rank_;
  bool gvt_request_received_;

  Time marked_event_time_;
};

#endif  // INCLUDED_MPI_POST_MASTER_H_
