### xyxy



Build:

```shell
bazel build -c dbg //...:all -s --sandbox_debug
```



Run all tests:

```shell
bazel test -c dbg //xyxy:compiler_test --test_output=all --action_env="GTEST_COLOR=1"
```



Run a specific test:

```shell
./bazel-bin/xyxy/compiler_test
```



Make code style
```shell
make style
```
