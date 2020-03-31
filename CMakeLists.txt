cmake_minimum_required(VERSION 3.5)

project(qian CXX)
set(CMAKE_CXX_STANDARD 17)
### Show detailed make command that runs
# set(CMAKE_VERBOSE_MAKEFILE ON)

### Print stuff
# include(CMakePrintHelpers)
# cmake_print_variables(MY_VARIABLE)
# cmake_print_properties(
#   TARGETS my_target
#   PROPERTIES POSITION_INDEPENDENT_CODE
# )

### Nice debug command
# cmake -S . -B build --trace-source=CMakeLists.txt --trace-expand -Wno-dev

# Switch debug or release build
# -DCMAKE_BUILD_TYPE=Debug

### Test
# using ctest command to test all

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_LIST_DIR}/CMake
  ${CMAKE_CURRENT_LIST_DIR}/CMake/copts
)

# include(AbseilInstallDirs)
include(CMakePackageConfigHelpers)
include(QianHelper)

# include current path
list(APPEND QIAN_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

option(QIAN_RUN_TESTS "If ON, tests will be run." ON)
option(QIAN_USE_GOOGLETEST "If ON, use google test." ON)

# cmake_print_variables(${QIAN_RUN_TESTS})
# cmake_print_variables(${QIAN_USE_GOOGLETEST})

if(${QIAN_RUN_TESTS})
  # enable CTest.  This will set BUILD_TESTING to ON unless otherwise specified
  # on the command line
  include(CTest)
  enable_testing()
endif()

## check targets
if(BUILD_TESTING)

  if(${QIAN_USE_GOOGLETEST})
    include(CMake/Googletest/DownloadGTest.cmake)
    set(byte_gtest_src_dir ${CMAKE_BINARY_DIR}/googletest-src)
    set(byte_gtest_build_dir ${CMAKE_BINARY_DIR}/googletest-build)
  endif()

  check_target(gtest)
  check_target(gtest_main)
  check_target(gmock)

  list(APPEND QIAN_TEST_COMMON_LIBRARIES
    gtest_main
    gtest
    gmock
    ${CMAKE_THREAD_LIBS_INIT}
)
endif()

add_subdirectory(qian)

# add_custom_target(graphviz ALL
#   COMMAND ${CMAKE_COMMAND} "--graphviz=qian.dot" .
#   COMMAND dot -Tpng qian.dot -o qian.png
#   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
# )
