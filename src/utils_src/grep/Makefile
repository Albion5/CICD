FLAGS= -Wall -Werror -Wextra
CC= gcc

all: build

build: s21_grep

s21_grep: s21_grep.c s21_grep.h
	$(CC) $(FLAGS) s21_grep.c -o s21_grep

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
	cd ./test_files && bash G_test_functionality.sh

test_valgrind: build
	cd ./test_files && bash G_test_valgrind.sh

test_leaks: build
	cd ./test_files && bash G_test_leaks.sh

clean: 
	rm -rf *.o *.log s21_grep

rebuild: clean build
