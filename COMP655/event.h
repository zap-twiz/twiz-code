#ifndef INCLUDED_EVENT_H__
#define INCLUDED_EVENT_H__

#include <limits.h>
#define MAX_TIME  LLONG_MAX

typedef unsigned long long Time;

#define EVENT_CUSTOM_TYPE         0x1000
#define EVENT_GVT_REQUEST_TYPE    (EVENT_CUSTOM_TYPE)
#define EVENT_REMOTE_MESSAGE_ACK  (EVENT_GVT_REQUEST_TYPE + 1)

class Event {
 public:
  int payload() const { return payload_; }
  void set_payload(int payload) { payload_ = payload; }

  int type() const { return type_; }
  void set_type(int type) { type_ = type; }

  void negate() { type_ *= -1; }

  bool is_negative() const { return type_ < 0; }
  bool is_positive() const { return type_ > 0; }

  Time send_time_stamp() const { return send_time_stamp_; }
  void set_send_time_stamp(Time stamp) { send_time_stamp_ = stamp; }

  Time receive_time_stamp() const { return receive_time_stamp_; }
  void set_receive_time_stamp(Time stamp) { receive_time_stamp_ = stamp; }

  int sending_process_id() const { return sending_process_id_; }
  void set_sending_process_id(int id) { sending_process_id_ = id; }

  int target_process_id() const { return target_process_id_; }
  void set_target_process_id(int id) { target_process_id_ = id; }

 private:
  Time send_time_stamp_;
  Time receive_time_stamp_;
  int payload_;
  int type_;
  int sending_process_id_;
  int target_process_id_;
};

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
}

#endif  // INCLUDED_EVENT_H__