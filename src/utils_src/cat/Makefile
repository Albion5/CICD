FLAGS= -Wall -Werror -Wextra
CC= gcc

all: build

build: s21_cat

s21_cat: s21_cat.c parser.c s21_cat.h parser.h
	$(CC) $(FLAGS) s21_cat.c parser.c -o s21_cat

clang_format:
	clang-format -i -style=google *.c *.h

check_style:
	@res="$$(clang-format -n -style=google *.c *.h 2>&1)"; \
	if [ -n "$$res" ]; then \
		echo "Code style check failed:"; \
		echo "$$res"; \
		exit 1; \
	else \
		echo "Code style check passed."; \
	fi


test_func: build
	cd ./test_files && bash C_test_functionality.sh

test_valgrind: build
	cd ./test_files && bash C_test_valgrind.sh

test_leaks: build
	cd ./test_files && bash C_test_leaks.sh
clean:
	rm -rf *.o *.log s21_cat

rebuild: clean build
