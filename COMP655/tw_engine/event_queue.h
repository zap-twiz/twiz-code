#ifndef INCLUDED_EVENT_QUEUE_H__
#define INCLUDED_EVENT_QUEUE_H__

#include "heap.h"
#include "event.h"

#include <functional>
#include <vector>

// A simple class abstracting a queue for event processing.
class EventQueue {
 public:
  // Register an event in the queue.
  void RegisterEvent(Event const & event);

  // Returns the set of events next to be processed.
  // The set contains all elements sharing the smallest receive time stamp.
  void NextEvents(std::vector<Event>* events);

  // Returns true if the queue is empty.
  bool empty() const { return heap_.empty(); }

  // Return the time-stamp of the next set of events to process.
  Time TimeOfNextEvent() const;
 private:

  typedef Heap<Event, std::less<Event> > EventHeap;
  EventHeap heap_;
};

#endif  // INCLUDED_EVENT_QUEUE_H__