TODO:

- ~Consider to add logging feature.~

- ~(NO NEED)Add a docker enviroment since valgrind has some issue running on latest osx so that I can do memory leak check~.

- Add cpplint for linting check.

- Add benchmark feature to test the speed of code.

- ~(NO NEED)Use abseil::string instead std::string~.

- ~Add a better way to debug token tokentype and prec order~.

- Find a better way to detet cycle inclusion.

- ~Debug parser process, write a stack like code to track call depth and draw info~

- ~Review all the places where pointer been used, use unique_ptr instead~.

- ~Use bazel to replace cmake, use glog lib~.

- Compiler error handing.

- Add a checker to do unittest for compiler.

- Add const atrributes for all variables and functions that expect not be changed.
