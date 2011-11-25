#ifndef INCLUDED_EVENT_H__
#define INCLUDED_EVENT_H__

#include <limits.h>
#define MAX_TIME  ULONG_MAX

// Convenient typedef for representing a time-stamp.
// NOTE:  It is assumed that the simulation time will not wrap MAX_TIME.
typedef unsigned long Time;

// Class representing an event sent by the system.
class Event {
 public:
  // Set/get the payload of the event.  To be determined by the application.
  int payload() const { return payload_; }
  void set_payload(int payload) { payload_ = payload; }

  // Set/get the type of the event.  To be determined by the application.
  // Note:  Events with negative type are considered anti-events.
  int type() const { return type_; }
  void set_type(int type) { type_ = type; }

  // Convert the event to an anti-event.
  void negate() { type_ *= -1; }

  bool is_negative() const { return type_ < 0; }
  bool is_positive() const { return type_ > 0; }

  // Time-stamp accessor functions.  Send is the time at which an LP sent
  // the event.  Receive is the time stamp at which the event is to be
  // processed by the receiving LP.
  Time send_time_stamp() const { return send_time_stamp_; }
  void set_send_time_stamp(Time stamp) { send_time_stamp_ = stamp; }

  Time receive_time_stamp() const { return receive_time_stamp_; }
  void set_receive_time_stamp(Time stamp) { receive_time_stamp_ = stamp; }

  // Each event tracks the id of the LP that sent it, and the LP to which
  // it is destined.
  int sending_process_id() const { return sending_process_id_; }
  void set_sending_process_id(int id) { sending_process_id_ = id; }

  int target_process_id() const { return target_process_id_; }
  void set_target_process_id(int id) { target_process_id_ = id; }

  // Marker fields, used in the GVT algorithm.
  bool is_marked() const { return !!marked_; }
  void set_marked(bool marked) { marked_ = marked; }

 private:
  Time send_time_stamp_;
  Time receive_time_stamp_;
  int payload_;
  int type_;
  int sending_process_id_;
  int target_process_id_;
  char marked_;
};

// Convenience less-than operator to allow events to be sorted
// by their receive time stamp.
inline bool operator<(Event const & e1, Event const & e2) {
  return e1.receive_time_stamp() < e2.receive_time_stamp();
}

inline bool operator==(Event const &e1, Event const & e2) {
  return e1.payload() == e2.payload() &&
      e1.type() == e2.type() &&
      e1.send_time_stamp() == e2.send_time_stamp() &&
      e1.receive_time_stamp() == e2.receive_time_stamp() &&
      e1.sending_process_id() == e2.sending_process_id() &&
      e1.target_process_id() == e2.target_process_id();

      // Disregard the marked setting of this event.  The marked field
      // does not factor into event equality.
      //e1.is_marked() == e2.is_marked();
}

#endif  // INCLUDED_EVENT_H__