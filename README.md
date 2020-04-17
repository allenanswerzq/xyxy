### xyxy



Build:

```shell
bazel build -c dbg //...:all -s --sandbox_debug
```



Run all tests:

```shell
bazel test -c dbg //...:all --test_output=all --action_env="GTEST_COLOR=1"
```



Run a specific test:

```shell
./bazel-bin/xyxy/compiler_test
```

Run a specific test with log info:

```shell
# Only show runtime debug logs.
GLOG_v=2 ./bazel-bin/xyxy/compiler_test

# Show very detailed debug logs.
GLOG_v=3 ./bazel-bin/xyxy/compiler_test
```


Make code style
```shell
make style
```
