#ifndef INCLUDED_WORK_BENCH_H_
#define INCLUDED_WORK_BENCH_H_

#include "chip.h"

#include <map>
#include <string>

class WorkBench {
 public:
  virtual ~WorkBench() {
    ClearBench();
  }

  ChipBuilder* builder(std::string const & name) const {
    BenchMap::const_iterator iter = bench_.find(name);
    if (iter != bench_.end()) {
      return iter->second;
    }
    return NULL;
  }

  void RegisterBuilder(ChipBuilder* builder) {
    ChipDescription const * description = builder->description();
    bench_[description->name()] = builder;
  }

 private:

  void ClearBench();

  typedef std::map<std::string, ChipBuilder*> BenchMap;
  BenchMap bench_;
};

void WorkBench::ClearBench() {
  BenchMap::iterator iter(bench_.begin()), end(bench_.end());
  for (; iter != end; ++iter) {
    delete iter->second;
  }

  bench_.clear();
}

#endif  // INCLUDED_WORK_BENCH_H_
