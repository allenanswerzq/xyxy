#ifndef XYXY_UTIL_H_
#define XYXY_UTIL_H_

#include <glog/logging.h>

namespace xyxy {

using ::google::LogMessage;
using ::google::LogSeverity;
using ::google::LogSeverityNames;

static const std::vector<std::string> kColors = {
    "\033[0m",
    "\033[0;36m",
    "\033[1;31m",
    "\033[1;32m",
};

enum class ColorType : int {
  NONE = 0,
  CYAN = 1,
  RED = 2,
  YELLOW = 3,
};

struct NewLogMessage : LogMessage {
  NewLogMessage(const char* file, const char* func, int line,
                LogSeverity severity, ColorType color = ColorType::NONE)
      : LogMessage(file, line, severity) {
    // Supress the glog logging prefix info.
    FLAGS_log_prefix = false;
    // Custom log prefix.
    std::string prefix;
    prefix += LogSeverityNames[severity][0];
    prefix += "[";
    prefix += std::string(file);
    prefix += ":";
    prefix += std::string(func);
    prefix += ":";
    prefix += std::to_string(line);
    prefix +=
        std::string(std::max(kAlignmentSize - (int)prefix.size(), 0), ' ');
    prefix += "] ";
    stream() << prefix << kColors[(size_t)color];
  }

  ~NewLogMessage() { stream() << kColors[(size_t)ColorType::NONE]; }

  static const int kAlignmentSize;
};

// TODO(): make this configurable or autofit terminal width.
const int NewLogMessage::kAlignmentSize = 45;

// Undef LOG from glog
#undef LOG

#define LOG(severity)                                                    \
  NewLogMessage(__FILE__, __func__, __LINE__, ::google::GLOG_##severity) \
      .stream()

#define LOG_WITH_COLOR(severity, color)                                  \
  NewLogMessage(__FILE__, __func__, __LINE__, ::google::GLOG_##severity, \
                color)                                                   \
      .stream()

#define LOG_IF_COLOR(severity, color, condition)                     \
  static_cast<void>(0), !(condition) ? (void)0                       \
                                     : google::LogMessageVoidify() & \
                                           LOG_WITH_COLOR(severity, color)

#define XYLOG(verboselevel, color) \
  LOG_IF_COLOR(INFO, color, VLOG_IS_ON(verboselevel))

#define LOGv XYLOG(1, ColorType::NONE)
#define LOGvv XYLOG(2, ColorType::NONE)
#define LOGvvv XYLOG(3, ColorType::NONE)

// Logging with color in red
#define LOGRv XYLOG(1, ColorType::RED)
#define LOGRvv XYLOG(2, ColorType::RED)
#define LOGRvvv XYLOG(3, ColorType::RED)

// Logging with color in cyan
#define LOGCv XYLOG(1, ColorType::CYAN)
#define LOGCvv XYLOG(2, ColorType::CYAN)
#define LOGCvvv XYLOG(3, ColorType::CYAN)

// Logging with color in yellow
#define LOGYv XYLOG(1, ColorType::YELLOW)
#define LOGYvv XYLOG(2, ColorType::YELLOW)
#define LOGYvvv XYLOG(3, ColorType::YELLOW)

}  // namespace xyxy

#endif  //
