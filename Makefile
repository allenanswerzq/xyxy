
TESTS = \
	./build/qian/qian_chunk_test \
	./build/qian/qian_debug_test

grind:
	valgrind --leak-check=full ${TESTS}

style:
	find qian -name "*.cc" -o -name "*.h" | xargs -t -I{} clang-format -i {}
	find stl -name "*.cc" -o -name "*.h" | xargs -t -I{} clang-format -i {}
