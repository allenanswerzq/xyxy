
# TESTS = \

# grind:
# 	valgrind --leak-check=full ${TESTS}

style:
	find xyxy -name "*.cc" -o -name "*.h" | xargs -t -I{} clang-format -i {}
