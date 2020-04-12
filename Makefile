
check:
	@./memory-check.sh

style:
	find xyxy -name "*.cc" -o -name "*.h" | xargs -t -I{} clang-format -i {}
