#include "status.h"

namespace xyxy {

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
  }
  else {
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
  }
  else {
    char tmp[30];
    const char* type;
    switch (code()) {
      case ErrorCode::CANCELLED:
        type = "Cancelled";
        break;
      case ErrorCode::UNKNOWN:
        type = "Unknown";
        break;
      case xyxy::ErrorCode::INVALID_ARGUMENT:
        type = "Invalid argument";
        break;
      case xyxy::ErrorCode::DEADLINE_EXCEEDED:
        type = "Deadline exceeded";
        break;
      case xyxy::ErrorCode::NOT_FOUND:
        type = "Not found";
        break;
      case xyxy::ErrorCode::ALREADY_EXISTS:
        type = "Already exists";
        break;
      case xyxy::ErrorCode::PERMISSION_DENIED:
        type = "Permission denied";
        break;
      case xyxy::ErrorCode::UNAUTHENTICATED:
        type = "Unauthenticated";
        break;
      case xyxy::ErrorCode::RESOURCE_EXHAUSTED:
        type = "Resource exhausted";
        break;
      case xyxy::ErrorCode::FAILED_PRECONDITION:
        type = "Failed precondition";
        break;
      case xyxy::ErrorCode::ABORTED:
        type = "Aborted";
        break;
      case xyxy::ErrorCode::OUT_OF_RANGE:
        type = "Out of range";
        break;
      case xyxy::ErrorCode::UNIMPLEMENTED:
        type = "Unimplemented";
        break;
      case xyxy::ErrorCode::INTERNAL:
        type = "Internal";
        break;
      case xyxy::ErrorCode::UNAVAILABLE:
        type = "Unavailable";
        break;
      case xyxy::ErrorCode::DATA_LOSS:
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
}  // namespace xyxy
