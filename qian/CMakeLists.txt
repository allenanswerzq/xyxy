qian_cc_library(
  NAME
    base
  HDRS
    # "status.h"
    # "base.h"
    # "stack.h"
    "chunk.h"
    # "logging.h"
  SRCS
    "chunk.cc"
    # "logging.cc"
    # "status.cc"
  COPTS
    ${QIAN_DEFAULT_COPTS}
  PUBLIC
)

# qian_cc_library(
#   NAME
#     qian
#   HDRS
#     "vm.h"
#     "type.h"
#     "inst.h"
#     "inst_impl.h"
#     "debug.h"
#     "scanner.h"
#     "parser.h"
#   SRCS
#     "vm.cc"
#     "debug.cc"
#     "inst.cc"
#     "scanner.cc"
#     "parser.cc"
#   COPTS
#     ${QIAN_DEFAULT_COPTS}
#   DEPS
#     qian::base
#   PUBLIC
# )

qian_cc_test(
  NAME
    type_test
  SRCS
    "type_test.cc"
  COPTS
    ${QIAN_TEST_COPTS}
  DEPS
    gmock_main
)

qian_cc_test(
  NAME
    chunk_test
  SRCS
    "chunk_test.cc"
  COPTS
    ${QIAN_TEST_COPTS}
  DEPS
    qian::base
    gmock_main
)

# qian_cc_test(
#   NAME
#     stack_test
#   SRCS
#     "stack_test.cc"
#   COPTS
#     ${QIAN_TEST_COPTS}
#   DEPS
#     qian::base
#     gmock_main
# )

# qian_cc_test(
#   NAME
#     vector_test
#   SRCS
#     "vector_test.cc"
#   COPTS
#     ${QIAN_TEST_COPTS}
#   DEPS
#     qian::base
#     gmock_main
# )

# qian_cc_test(
#   NAME
#     scanner_test
#   SRCS
#     "scanner_test.cc"
#   COPTS
#     ${QIAN_TEST_COPTS}
#   DEPS
#     qian::base
#     qian::qian
#     gmock_main
# )

# qian_cc_test(
#   NAME
#     parser_test
#   SRCS
#     "parser_test.cc"
#   COPTS
#     ${QIAN_TEST_COPTS}
#   DEPS
#     qian::base
#     qian::qian
#     gmock_main
# )
