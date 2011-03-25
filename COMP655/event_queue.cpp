
#include "stdafx.h"
#include "event_queue.h"
#include "event.h"

#include <algorithm>
#include <assert.h>
#include <iostream>

namespace {

void PositiveNegativeIntersect(std::vector<Event> const & pos_events,
                               std::vector<Event> const & neg_events,
                               std::vector<Event>* intersection ) {
  intersection->clear();

  std::vector<Event>::const_iterator neg_iter(neg_events.begin()),
    neg_end(neg_events.end());
  for (; neg_iter != neg_end; ++neg_iter) {
    Event positive_equivalent = *neg_iter;
    positive_equivalent.negate();

    std::vector<Event>::const_iterator found_pos = std::find(
        pos_events.begin(), pos_events.end(), positive_equivalent);
    if (found_pos != pos_events.end()) {
      intersection->push_back(positive_equivalent);
    }
  }
}

void DoAnnihilation(std::vector<Event>& pos_events,
                    std::vector<Event>& neg_events,
                    std::vector<Event> const & intersection) {
  std::vector<Event>::const_iterator anihil_iter(intersection.begin()),
    anihil_end(intersection.end());
  for (; anihil_iter != anihil_end; ++anihil_iter) {
    std::vector<Event>::iterator found_pos =
        std::find(pos_events.begin(), pos_events.end(), *anihil_iter);
    assert(found_pos != pos_events.end());

    Event negative_event = *anihil_iter;
    negative_event.negate();
    std::vector<Event>::iterator found_neg =
        std::find(neg_events.begin(), neg_events.end(), negative_event);
    assert(found_neg != neg_events.end());

    pos_events.erase(found_pos);
    neg_events.erase(found_neg);
  }
}

}  // namespace

void EventQueue::RegisterEvent(Event const & event) {
  if (heap_.size() > 100) {
    std::cout << "QueueSize: " << heap_.size() << std::endl;
  }
  heap_.insert(event);
}

void EventQueue::NextEvents(std::vector<Event>* pos_events,
                            std::vector<Event>* neg_events) {
  assert(NULL != pos_events && NULL != neg_events);

  pos_events->clear();
  neg_events->clear();
  if (heap_.empty())
    return;

  Event top_event = heap_.RemoveTop();

  if (top_event.is_positive()) {
    pos_events->push_back(top_event);
  } else {
    neg_events->push_back(top_event);
  }

  while (!heap_.empty() &&
         heap_.top().receive_time_stamp() == top_event.receive_time_stamp()) {
    Event candidate = heap_.RemoveTop();
    if (candidate.is_positive()) {
      pos_events->push_back(candidate);
    } else {
      neg_events->push_back(candidate);
    }
  }

  std::vector<Event> to_annihilate;
  PositiveNegativeIntersect(*pos_events, *neg_events, &to_annihilate);

  DoAnnihilation(*pos_events, *neg_events, to_annihilate);
}
