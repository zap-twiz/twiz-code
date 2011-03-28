
#include "stdafx.h"
#include "event_queue.h"
#include "event.h"

#include <algorithm>
#include <assert.h>
#include <iostream>

void EventQueue::RegisterEvent(Event const & event) {
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

Time EventQueue::TimeOfNextEvent() const {
  if (heap_.empty()) {
    return MAX_TIME; // TODO:  Is this correct?
  } else {
    return heap_.top().receive_time_stamp();
  }
}
