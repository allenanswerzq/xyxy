#ifndef QIAN_STATUS_H_
#define QIAN_STATUS_H_

#include "base.h"
#include <functional>

namespace qian {

typedef enum {
  OK = 0,
  CANCELLED = 1,
  UNKNOWN = 2,
  INVALID_ARGUMENT = 3,
  DEADLINE_EXCEEDED = 4,
  NOT_FOUND = 5,
  ALREADY_EXISTS = 6,
  PERMISSION_DENIED = 7,
  UNAUTHENTICATED = 16,
  RESOURCE_EXHAUSTED = 8,
  FAILED_PRECONDITION = 9,
  ABORTED = 10,
  OUT_OF_RANGE = 11,
  UNIMPLEMENTED = 12,
  INTERNAL = 13,
  UNAVAILABLE = 14,
  DATA_LOSS = 15,
  COMPILE_ERROR = 16,
  RUNTIME_ERROR = 17,
} ErrorCode;

// A Status class adapted from TensorFlow source code.
class Status {
 public:
  // Create a success status.
  Status() : state_(NULL) {}
  ~Status() { delete state_; }

  // Create a status with the specified error code and msg as a
  // human-readable string containing more detailed information.
  Status(ErrorCode code, string msg);

  // Copy the specified status.
  Status(const Status& s);
  void operator=(const Status& s);

  static Status OK() { return Status(); }

  // Returns true iff the status indicates success.
  bool ok() const { return (state_ == NULL); }

  ErrorCode code() const {
    return ok() ? ErrorCode::OK : state_->code;
  }

  const string& error_message() const {
    return ok() ? empty_string() : state_->msg;
  }

  bool operator==(const Status& x) const;
  bool operator!=(const Status& x) const;

  // If `ok()`, stores `new_status` into `*this`.  If `!ok()`,
  // preserves the current status, but may augment with additional
  // information about `new_status`.
  //
  // Convenient way of keeping track of the first error encountered.
  // Instead of:
  //   `if (overall_status.ok()) overall_status = new_status`
  // Use:
  //   `overall_status.Update(new_status);`
  void Update(const Status& new_status);

  // Return a string representation of this status suitable for
  // printing. Returns the string `"OK"` for success.
  string ToString() const;

 private:
  static const string& empty_string();

  struct State {
    ErrorCode code;
    string msg;
  };

  // OK status has a `NULL` state_.  Otherwise, `state_` points to
  // a `State` structure containing the error code and message(s)
  State* state_;

  void SlowCopyFrom(const State* src);
};

// -----------------------------------------------------------------

inline Status::Status(const Status& s)
    : state_((s.state_ == NULL) ? NULL : new State(*s.state_)) {}

inline void Status::operator=(const Status& s) {
  // The following condition catches both aliasing (when this == &s),
  // and the common case where both s and *this are ok.
  if (state_ != s.state_) {
    SlowCopyFrom(s.state_);
  }
}

inline bool Status::operator==(const Status& x) const {
  return (this->state_ == x.state_) || (ToString() == x.ToString());
}

inline bool Status::operator!=(const Status& x) const { return !(*this == x); }

std::ostream& operator<<(std::ostream& os, const Status& x);

typedef std::function<void(const Status&)> StatusCallback;

} // namespace qian

#endif // QIAN_STATUS_H
