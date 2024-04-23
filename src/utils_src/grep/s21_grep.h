#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdio.h>

typedef struct Flags {
  int show_file_names;   // -l
  int count_lines;       // -c
  int ignore_case;       // -i
  int invert_search;     // -v
  int number_lines;      // -n
  int hide_file_names;   // -h
  int show_all_matches;  // -o
  int hide_errors;       // -s
  int search_pattern;    // -e -f
  char compile_flags;
} Flags;

const char *short_options = "f:e:lcivnhos";
const struct option long_options[] = {{NULL, 0, NULL, 0}};

int s21_grep(int argc, char *argv[]);
void expand_pattern(Flags *flags, char *pattern, char *substring);
int get_pattern_from_file(Flags *flags, char *filename, char *pattern);
void print_line_number(Flags flags, int line_number);
void print_file_name(Flags flags, char *filename);
void print_line(Flags flags, char *line, char *filename, int *end_search);
void print_count(Flags flags, int count, char *filename);
void find_match(FILE *file, char *filename, Flags flags, regex_t reegex);
int comp_expression(FILE *file, char *filename, Flags flags, char *pattern);
int work_with_file(char *filename, Flags flags, char *pattern);
int handle_files(int argc, char *argv[], Flags *flags, char *pattern);
int handle_pattern(int argc, char *argv[], Flags flags, char *pattern);
void set_e(Flags *flags, char *pattern);
int set_f(Flags *flags, char *pattern);
void set_l(Flags *flags);
void set_c(Flags *flags);
void set_i(Flags *flags);
void set_v(Flags *flags);
void set_n(Flags *flags);
void set_h(Flags *flags);
void set_o(Flags *flags);
void set_s(Flags *flags);
int parse_arguments(int argc, char *argv[], Flags *flags, char *pattern);
void print_usage_error();
void print_directory_error(char *filename);

#endif  // SRC_GREP_S21_GREP_H_
