#include "status.h"

namespace qian {

Status::Status(ErrorCode code, string msg) {
  assert(code != ErrorCode::OK);
  state_ = new State;
  state_->code = code;
  state_->msg = msg;
}

void Status::Update(const Status& new_status) {
  if (ok()) {
    *this = new_status;
  }
}

void Status::SlowCopyFrom(const State* src) {
  delete state_;
  if (src == nullptr) {
    state_ = nullptr;
  } else {
    state_ = new State(*src);
  }
}

const string& Status::empty_string() {
  static string* empty = new string;
  return *empty;
}

string Status::ToString() const {
  if (state_ == NULL) {
    return "OK";
  } else {
    char tmp[30];
    const char* type;
    switch (code()) {
      case ErrorCode::CANCELLED:
        type = "Cancelled";
        break;
      case ErrorCode::UNKNOWN:
        type = "Unknown";
        break;
      case qian::ErrorCode::INVALID_ARGUMENT:
        type = "Invalid argument";
        break;
      case qian::ErrorCode::DEADLINE_EXCEEDED:
        type = "Deadline exceeded";
        break;
      case qian::ErrorCode::NOT_FOUND:
        type = "Not found";
        break;
      case qian::ErrorCode::ALREADY_EXISTS:
        type = "Already exists";
        break;
      case qian::ErrorCode::PERMISSION_DENIED:
        type = "Permission denied";
        break;
      case qian::ErrorCode::UNAUTHENTICATED:
        type = "Unauthenticated";
        break;
      case qian::ErrorCode::RESOURCE_EXHAUSTED:
        type = "Resource exhausted";
        break;
      case qian::ErrorCode::FAILED_PRECONDITION:
        type = "Failed precondition";
        break;
      case qian::ErrorCode::ABORTED:
        type = "Aborted";
        break;
      case qian::ErrorCode::OUT_OF_RANGE:
        type = "Out of range";
        break;
      case qian::ErrorCode::UNIMPLEMENTED:
        type = "Unimplemented";
        break;
      case qian::ErrorCode::INTERNAL:
        type = "Internal";
        break;
      case qian::ErrorCode::UNAVAILABLE:
        type = "Unavailable";
        break;
      case qian::ErrorCode::DATA_LOSS:
        type = "Data loss";
        break;
      default:
        snprintf(tmp, sizeof(tmp), "Unknown code(%d)",
                 static_cast<int>(code()));
        type = tmp;
        break;
    }
    string result(type);
    result += ": ";
    result += state_->msg;
    return result;
  }
}

std::ostream& operator<<(std::ostream& os, const Status& x) {
  os << x.ToString();
  return os;
}
} // namespace qian
