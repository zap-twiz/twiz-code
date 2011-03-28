#ifndef INCLUDED_MPI_POST_MASTER_H_
#define INCLUDED_MPI_POST_MASTER_H_

#include "post_master.h"

#include <map>

class MPIPostMaster : public PartitionedPostMaster {
 public:
  virtual ~MPIPostMaster() {}
  virtual void SendMessage(Event const & event);

  void RegisterLPRank(int lp_id, int external_rank) {
    lp_rank_map_[lp_id] = external_rank;
  }

  bool LocalVirtualTimeContribution(Time* time) const {
    // The default behaviour is that the post-master has no
    // contribution to the GVT computations
    return false;
  }
 private:
  std::map<int, int>  lp_rank_map_;
};

#endif  // INCLUDED_MPI_POST_MASTER_H_
