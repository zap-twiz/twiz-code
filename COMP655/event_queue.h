#ifndef INCLUDED_EVENT_QUEUE_H__
#define INCLUDED_EVENT_QUEUE_H__

#include "heap.h"
#include "event.h"

#include <functional>
#include <vector>

class EventQueue {
 public:
  void RegisterEvent(Event const & event);

  void NextEvents(std::vector<Event>* events);

  bool empty() const { return heap_.empty(); }
 private:

  typedef Heap<Event, std::less<Event> > EventHeap;
  EventHeap heap_;
};

#endif  // INCLUDED_EVENT_QUEUE_H__