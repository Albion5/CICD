#include "parser.h"

#include <stdio.h>
#include <string.h>

void print_long_flag_error(char *option) {
  // flag error
  fprintf(stderr, "%s: %s '%s'\n", "s21_cat", "unrecognized option", option);
}

void print_single_flag_error(char option) {
  // flag error
  fprintf(stderr, "%s: %s '%c'\n", "s21_cat", "--invalid option", option);
}

void print_file_error(char *filename) {
  // file error
  fprintf(stderr, "%s: %s: %s\n", "s21_cat", filename,
          "No such file or directory");
}

void print_error(Errors error) {
  switch (error.error_code) {
    case 1:
      print_single_flag_error(error.symbol);
      break;
    case 2:
      print_long_flag_error(error.string);
      break;
    case 3:
      print_file_error(error.string);
      break;
  }
}

void set_no_error(Errors *error) { error->error_code = 0; }

void set_sflag_error(Errors *error, char option) {
  error->error_code = 1;
  error->symbol = option;
}

void set_lflag_error(Errors *error, char *option) {
  error->error_code = 2;
  strcpy(error->string, option);
}
void set_file_error(Errors *error, char *filename) {
  error->error_code = 3;
  strcpy(error->string, filename);
}

void set_b(Flags *flags) {
  flags->number_non_empty = 1;
  flags->number_all = 0;
}

void set_e(Flags *flags) {
  flags->show_end = 1;
  flags->show_unprintable = 1;
}
void set_E(Flags *flags) { flags->show_end = 1; }

void set_v(Flags *flags) { flags->show_unprintable = 1; }

void set_n(Flags *flags) {
  if ((!flags->number_non_empty)) {
    flags->number_all = 1;
  }
}

void set_s(Flags *flags) { flags->squeeze = 1; }

void set_t(Flags *flags) {
  flags->show_tab = 1;
  flags->show_unprintable = 1;
}

void set_T(Flags *flags) { flags->show_tab = 1; }

int check_single_flag(char symbol, Flags *flags, Errors *error) {
  int res = 1;
  switch (symbol) {
    case 'b': /* Number non empty lines */
      set_b(flags);
      break;
    case 'e': /* equivalent to -vE */
      set_e(flags);
      break;
    case 'E': /* Display $ at end of each line */
      set_E(flags);
      break;
    case 'n': /* Number all output lines */
      set_n(flags);
      break;
    case 's': /* Suppress repeated empty output lines */
      set_s(flags);
      break;
    case 't': /* equivalent to -vT */
      set_t(flags);
      break;
    case 'T': /* Display TAB characters as ^I */
      set_T(flags);
      break;
    case 'v': /* Show unprinted characters */
      set_v(flags);
      break;
    default: /* Not valid char */
      res = 0;
      set_sflag_error(error, symbol);
  }
  return res;
}

int check_long_flag(char *string, Flags *flags, Errors *error) {
  int res = 1;
  if (strcmp(string, "--number-nonblank") == 0) {
    set_b(flags);
  } else if (!strcmp(string, "--squeeze-blank")) {
    set_s(flags);
  } else if (!strcmp(string, "--number")) {
    set_n(flags);
  } else if (strlen(string) != 2) {
    res = 0;
    set_lflag_error(error, string);
  }
  return res;
}

/* Check if a string consists valid flags
  Return 0 if flag error, 1 if only valid flag(s) found, 2 if not a flag */
int check_flag(char *string, Flags *flags, Errors *error) {
  int res = 1;
  if (string[0] == '-') {
    if (string[1] == '-') {
      res = check_long_flag(string, flags, error);
    } else {
      int i = 1;
      int len = strlen(string);
      while (i < len && res) {
        res = check_single_flag(string[i], flags, error);
        i++;
      }
    }
  } else {
    // flag not found
    res = 2;
  }
  return res;
}

/* Find valid flags in command line args and save indexes of args with
   potential file names */
void find_flags(int argc, char *argv[], Flags *flags, int *file_indexes,
                int *count, Errors *error) {
  // Find all the flags
  int flag = 1;
  // Arg's index in argv
  int i = 1;
  // Index in file_indexes
  int file_index = 0;
  // While there are args in argv and stop flag is off
  while ((i < argc) && flag) {
    // Check if it's a flag
    flag = check_flag(argv[i], flags, error);
    // If it's not a flag, add arg's index to file_indexes
    if (flag == 2) {
      *(file_indexes + file_index) = i;
      // Increase number of elements in files_indexes
      *count += 1;
      file_index += 1;
    }
    // Increase arg's index
    i++;
  }
  // If no error occured, update error struct
  if (flag) {
    set_no_error(error);
  }
}
