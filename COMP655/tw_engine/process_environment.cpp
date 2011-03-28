
#include "stdafx.h"
#include "process_environment.h"
#include "logical_process.h"

#include <assert.h>

ProcessEnvironment::~ProcessEnvironment() {
  ProcessMap::iterator iter(processes_.begin()),
      end(processes_.end());
  for (; iter != end; ++iter)
    delete iter->second;
}

LogicalProcess* ProcessEnvironment::process(int id) {
  ProcessMap::iterator proc = processes_.find(id);
  if (proc == processes_.end())
    return NULL;
  else 
    return proc->second;
}

void ProcessEnvironment::RegisterLogicalProcess(LogicalProcess* proc) {
  assert(processes_.find(proc->id()) == processes_.end());
  processes_[proc->id()] = proc;
}

void ProcessEnvironment::Send(Event const& event) {
  if (post_master_) {
    post_master_->SendMessage(event);
  } else {
    event_queue().RegisterEvent(event);
  }
#if 0
  if (!post_master_ ||
      post_master_->IsLPLocal(event.target_process_id())) {
    event_queue().RegisterEvent(event);
  } else {
    post_master_->SendMessage(/* arguments */);
  }
#endif
}

Time ProcessEnvironment::VirtualTime() const {
  Time local_time = message_queue_.TimeOfNextEvent();

  ProcessMap::const_iterator iter(processes_.begin()),
    end(processes_.end());
  for (; iter != end; ++iter) {
    Time proc_time = iter->second->LocalVirtualTime();
    if (proc_time < local_time) {
      local_time = proc_time;
    }
  }

  return local_time;
}

void ProcessEnvironment::FossilCollect(Time gvt) {
  ProcessMap::iterator iter(processes_.begin()),
    end(processes_.end());
  for (; iter != end; ++iter) {
    iter->second->FossilCollect(gvt);
  }
}
