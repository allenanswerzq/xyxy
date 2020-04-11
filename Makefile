
TESTS = \
	./bazel-bin/xyxy/chunk_test \
	./bazel-bin/xyxy/compiler_test \
	./bazel-bin/xyxy/hash_table_test \
	./bazel-bin/xyxy/inst_test \
	./bazel-bin/xyxy/list_test \
	./bazel-bin/xyxy/scanner_test \
	./bazel-bin/xyxy/stack_test \
	./bazel-bin/xyxy/type_test \

check:
	valgrind --leak-check=full ${TESTS}

style:
	find xyxy -name "*.cc" -o -name "*.h" | xargs -t -I{} clang-format -i {}
