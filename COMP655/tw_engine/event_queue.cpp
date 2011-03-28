
#include "stdafx.h"
#include "event_queue.h"
#include "event.h"

#include <algorithm>
#include <assert.h>
#include <iostream>

void EventQueue::RegisterEvent(Event const & event) {
#if 0
  if (heap_.size() > 100) {
    std::cout << "QueueSize: " << heap_.size() << std::endl;
  }
#endif
  heap_.insert(event);
}

void EventQueue::NextEvents(std::vector<Event>* events) {
  assert(NULL != events);

  if (heap_.empty())
    return;

  Event top = heap_.RemoveTop();
  events->push_back(top);

  while (!heap_.empty() &&
         heap_.top().receive_time_stamp() == top.receive_time_stamp()) {
    events->push_back(heap_.RemoveTop());
  }
}
