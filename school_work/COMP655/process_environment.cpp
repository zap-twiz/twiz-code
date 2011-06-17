
#include "stdafx.h"
#include "process_environment.h"
#include "logical_process.h"

ProcessEnvironment::~ProcessEnvironment() {
  ProcessVector::iterator iter(processes_.begin()),
      end(processes_.end());
  for (; iter != end; ++iter)
    delete *iter;
}

void ProcessEnvironment::Send(Event const& event) {
  event_queue().RegisterEvent(event);
}

Time ProcessEnvironment::VirtualTime() const {
  Time local_time = MAX_TIME;

  ProcessVector::const_iterator iter(processes_.begin()),
    end(processes_.end());
  for (; iter != end; ++iter) {
    if ((*iter)->LogicalTime() < local_time) {
      local_time = (*iter)->LogicalTime();
    }
  }

  return local_time;
}
