
TESTS = \
	./build/qian/qian_chunk_test \
	./build/qian/qian_debug_test

grind:
	valgrind --leak-check=full ${TESTS}