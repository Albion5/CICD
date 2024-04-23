#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int res = 1;
  if (argc > 2) {
    if (s21_grep(argc, argv)) {
      res = 0;
    }
  } else {
    print_usage_error();
  }
  return res;
}

int s21_grep(int argc, char *argv[]) {
  int res = 0;
  Flags flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, REG_EXTENDED};
  char pattern[4096];
  if (parse_arguments(argc, argv, &flags, pattern)) {
    if (handle_pattern(argc, argv, flags, pattern)) {
      res = handle_files(argc, argv, &flags, pattern);
    }
  }
  return res;
}

void expand_pattern(Flags *flags, char *pattern, char *substring) {
  if (!flags->search_pattern) {
    strcpy(pattern, substring);
  } else {
    strcat(pattern, "|");
    strcat(pattern, substring);
  }
  flags->search_pattern = 1;
}

int get_pattern_from_file(Flags *flags, char *filename, char *pattern) {
  int res = 1;
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    char substring[4096];
    while (fgets(substring, 4095, file)) {
      substring[strcspn(substring, "\n")] = 0;
      expand_pattern(flags, pattern, substring);
    }
    fclose(file);
  } else {
    res = 0;
    print_directory_error(filename);
  }
  return res;
}

void print_line_number(Flags flags, int line_number) {
  if (flags.number_lines && !(flags.invert_search && flags.show_all_matches)) {
    printf("%d:", line_number);
  }
}

void print_file_name(Flags flags, char *filename) {
  if (!flags.hide_file_names && !flags.count_lines && !flags.show_file_names &&
      !(flags.invert_search && flags.show_all_matches)) {
    printf("%s:", filename);
  }
}

void print_line(Flags flags, char *line, char *filename, int *end_search) {
  if (flags.show_file_names) {
    *end_search = 1;
    printf("%s\n", filename);
  } else if (!flags.count_lines) {
    printf("%s\n", line);
  }
}

void print_count(Flags flags, int count, char *filename) {
  if (flags.count_lines) {
    if (!flags.hide_file_names) {
      printf("%s:", filename);
    }
    printf("%d\n", count);
  }
}

void find_match(FILE *file, char *filename, Flags flags, regex_t reegex) {
  char buffer[4096];
  int end_search = 0;
  int line_number = 0;
  int count = 0;
  regmatch_t pmatch[1];
  // Searching inside the contents of the entire file
  while (fgets(buffer, 4095, file) && !end_search) {
    line_number++;
    int start_flag = 0;
    buffer[strcspn(buffer, "\n")] = 0;
    char *cursor = buffer;
    // Searching inside one line
    int found_match = 0;
    while ((regexec(&reegex, cursor, 1, pmatch, start_flag) ==
            flags.invert_search) &&
           !found_match) {
      count += 1;
      print_file_name(flags, filename);
      print_line_number(flags, line_number);

      if (!flags.show_all_matches) {
        // Print whole line
        print_line(flags, buffer, filename, &end_search);
        found_match = 1;
      } else {
        if (flags.invert_search) {
          found_match = 1;
        } else {
          // Print one match and move cursor further inside a line
          int size = pmatch[0].rm_eo - pmatch[0].rm_so;
          printf("%.*s\n", size, cursor + pmatch[0].rm_so);
          cursor += pmatch[0].rm_eo;
          start_flag = REG_NOTBOL;
        }
      }
    }
  }
  print_count(flags, count, filename);
}

int comp_expression(FILE *file, char *filename, Flags flags, char *pattern) {
  int success = 1;
  regex_t reegex;

  int res = regcomp(&reegex, pattern, flags.compile_flags);
  if (res == 0) {
    find_match(file, filename, flags, reegex);
    regfree(&reegex);
  } else {
    // Print reg compilation error
    success = 0;
    regerror(res, &reegex, pattern, 120);
    fprintf(stderr, "s21_grep: %s\n", pattern);
  }
  return success;
}

int work_with_file(char *filename, Flags flags, char *pattern) {
  int res = 1;
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    res = comp_expression(file, filename, flags, pattern);
    fclose(file);
  } else {
    if (!flags.hide_errors) {
      print_directory_error(filename);
    }
  }
  return res;
}

int handle_files(int argc, char *argv[], Flags *flags, char *pattern) {
  int res = 1;
  int number_of_files = argc - optind;
  if (number_of_files < 1) {
    res = 0;
    print_usage_error();
  } else {
    if (number_of_files == 1) {
      flags->hide_file_names = 1;
    }
    int i = optind;
    while (i < argc && res) {
      res = work_with_file(argv[i], *flags, pattern);
      i++;
    }
  }
  return res;
}

int handle_pattern(int argc, char *argv[], Flags flags, char *pattern) {
  int res = 1;
  if (!flags.search_pattern) {
    if (optind < argc) {
      strcpy(pattern, argv[optind++]);
    } else {
      res = 0;
      print_usage_error();
    }
  }
  return res;
}

void set_e(Flags *flags, char *pattern) {
  expand_pattern(flags, pattern, optarg);
}
int set_f(Flags *flags, char *pattern) {
  return get_pattern_from_file(flags, optarg, pattern);
}
void set_l(Flags *flags) {
  flags->show_file_names = 1;
  flags->count_lines = 0;
  flags->number_lines = 0;
  flags->show_all_matches = 0;
}
void set_c(Flags *flags) {
  if (!flags->show_file_names) {
    flags->count_lines = 1;
    flags->show_all_matches = 0;
    flags->number_lines = 0;
  }
}
void set_i(Flags *flags) {
  flags->ignore_case = 1;
  flags->compile_flags |= REG_ICASE;
}
void set_v(Flags *flags) { flags->invert_search = REG_NOMATCH; }

void set_n(Flags *flags) {
  if (!flags->count_lines && !flags->show_file_names) {
    flags->number_lines = 1;
  }
}
void set_h(Flags *flags) {
  if (!flags->show_file_names) {
    flags->hide_file_names = 1;
  }
}
void set_o(Flags *flags) {
  if (!flags->show_file_names && !flags->count_lines) {
    flags->show_all_matches = 1;
  }
}
void set_s(Flags *flags) { flags->hide_errors = 1; }

int parse_arguments(int argc, char *argv[], Flags *flags, char *pattern) {
  int option;
  int res = 1;
  while ((option = getopt_long(argc, argv, short_options, long_options,
                               NULL)) != -1) {
    switch (option) {
      case 'f':
        if (!set_f(flags, pattern)) {
          res = 0;
        }
        break;
      case 'e':
        set_e(flags, pattern);
        break;
      case 'l':
        set_l(flags);
        break;
      case 'c':
        set_c(flags);
        break;
      case 'i':
        set_i(flags);
        break;
      case 'v':
        set_v(flags);
        break;
      case 'n':
        set_n(flags);
        break;
      case 'h':
        set_h(flags);
        break;
      case 'o':
        set_o(flags);
        break;
      case 's':
        set_s(flags);
        break;
      default:
        res = 0;
    }
  }
  return res;
}

void print_usage_error() {
  fprintf(stderr, "Usage: ./s21_grep [OPTION]... PATTERNS [FILE]...\n");
}

void print_directory_error(char *filename) {
  fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
}
