cmake_minimum_required(VERSION 3.5)

project(qian CXX)
set(CMAKE_CXX_STANDARD 11)

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

### Switch debug or release build
# -DCMAKE_BUILD_TYPE=Debug

### Generate compile_command.json
# -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1

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

# Enable CTest.
include(CTest)
enable_testing()

include(CMake/Googletest/DownloadGTest.cmake)
set(byte_gtest_src_dir ${CMAKE_BINARY_DIR}/googletest-src)
set(byte_gtest_build_dir ${CMAKE_BINARY_DIR}/googletest-build)

check_target(gtest)
check_target(gtest_main)
check_target(gmock)

list(APPEND QIAN_TEST_COMMON_LIBRARIES
  gtest_main
  gtest
  gmock
  ${CMAKE_THREAD_LIBS_INIT}
)

add_subdirectory(qian)

# add_custom_target(graphviz ALL
#   COMMAND ${CMAKE_COMMAND} "--graphviz=qian.dot" .
#   COMMAND dot -Tpng qian.dot -o qian.png
#   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
# )
