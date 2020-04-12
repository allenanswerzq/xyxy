#!/usr/bin/env bash

tests=`find bazel-bin/xyxy -maxdepth 1 -name "*_test" `
for test in $tests; do
  valgrind --leak-check=full "$test";
done
