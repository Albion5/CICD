#include "s21_cat.h"

#include <stdio.h>
#include <string.h>

#include "parser.h"

int main(int argc, char *argv[]) {
  int res = 0;
  // Initialize flags end error structs
  Flags flags = {0, 0, 0, 0, 0, 0, 0};
  Errors error;
  if (argc > 1) {
    // Mode with args
    res = parse_args(argc, argv, &flags, &error);
  } else {
    // Mode without args
    s21_cat(stdin, 0, &flags);
  }
  return res;
}

int is_Tab_char(unsigned char cur_char) { return cur_char == 9; }

int is_NEL_char(unsigned char cur_char) { return cur_char == 10; }

void handle_unprintable(unsigned char cur_char) {
  if (cur_char <= 31) {
    if (is_Tab_char(cur_char) || is_NEL_char(cur_char)) {
      printf("%c", cur_char);
    } else {
      printf("^%c", cur_char + 64);
    }
  } else if (cur_char < 127) {
    printf("%c", cur_char);
  } else if (cur_char == 127) {
    printf("^?");
  } else if (cur_char < 160) {
    printf("M-^%c", cur_char - 64);
  } else {
    if (cur_char == 255) {
      printf("M-^?");
    } else {
      printf("M-%c", cur_char - 128);
    }
  }
}

void print_new_line(Flags *flags) {
  if (flags->number_all) {
    printf("%6d\t", ++(flags->count));
  }
  if (flags->show_end) {
    printf("$");
  }
  printf("\n");
}

void print_with_flags(const unsigned char *buffer, int len, Flags *flags,
                      int *empty_repeat) {
  unsigned char cur_char = buffer[0];
  // Handle squeeze
  if (flags->squeeze) {
    if (is_NEL_char(cur_char)) {
      *empty_repeat += 1;
    } else {
      *empty_repeat = 0;
    }
  }
  // If the first char is not a new line char
  if (!is_NEL_char(cur_char)) {
    if (flags->number_all || flags->number_non_empty) {
      printf("%6d\t", ++(flags->count));
    }
    for (int i = 0; i < len; i++) {
      cur_char = buffer[i];
      if (flags->show_end && (is_NEL_char(cur_char))) {
        printf("$%c", cur_char);
      } else if (flags->show_tab && is_Tab_char(cur_char)) {
        printf("^I");
      } else if (flags->show_unprintable) {
        handle_unprintable(cur_char);
      } else {
        printf("%c", cur_char);
      }
    }
    // If the first char is a new line char
  } else {
    if (*empty_repeat < 2) {
      print_new_line(flags);
    }
  }
}

int get_number_of_flags(Flags flags) {
  int flags_found = flags.number_all + flags.number_non_empty + flags.squeeze;
  flags_found += flags.show_tab + flags.show_end + flags.show_unprintable;
  return flags_found;
}

int read_line(unsigned char *buffer, FILE *source, int *len) {
  int read = 1;
  *len = 0;
  int i = -1;
  buffer[0] = '\0';
  int symbol = fgetc(source);

  while (symbol != EOF && symbol != 10) {
    *len = *len + 1;
    buffer[++i] = (unsigned char)symbol;
    symbol = fgetc(source);
  }
  if (symbol == EOF) {
    read = 0;
  } else {
    buffer[++i] = (unsigned char)symbol;
    *len = *len + 1;
  }
  buffer[++i] = '\0';
  return read;
}

void s21_cat(FILE *source, int with_flags, Flags *flags) {
  unsigned char buffer[8096];
  int empty_repeat = 0;
  int len = 0;

  while (read_line(buffer, source, &len)) {
    // No flags mode
    if (!with_flags) {
      for (int i = 0; i < len; i++) {
        putchar(buffer[i]);
      }
    } else {
      print_with_flags(buffer, len, flags, &empty_repeat);
    }
  }
  if (len != 0) {
    if (!with_flags) {
      for (int i = 0; i < len; i++) {
        putchar(buffer[i]);
      }
    } else {
      print_with_flags(buffer, len, flags, &empty_repeat);
    }
  }
}

int parse_args(int argc, char *argv[], Flags *flags, Errors *error) {
  int file_indexes[100];
  int count = 0;
  int res = 1;
  // Find all the flags
  find_flags(argc, argv, flags, file_indexes, &count, error);

  // If no flag error
  if (error->error_code == 0) {
    // Stdin mode
    if ((count == 0)) {
      s21_cat(stdin, 1, flags);
      // File mode
    } else {
      find_files(argv, flags, file_indexes, count, error);
    }
    // Print flag error
  } else {
    print_error(*error);
    res = 0;
  }
  return res;
}

// Try opening a file, then print out it's original or updated content
void work_with_file(char *filename, Errors *error, Flags *flags,
                    int flags_found) {
  FILE *file = NULL;
  file = fopen(filename, "r");
  // If file cannot be opened
  if (file == NULL) {
    // Update an error struct and print file error
    set_file_error(error, filename);
    print_error(*error);
    // If arg is opened
  } else {
    // Work with file content
    s21_cat(file, flags_found, flags);
    // Close file
    fclose(file);
  }
}

void find_files(char *argv[], Flags *flags, int *file_indexes, int count,
                Errors *error) {
  int i = 0;
  int flags_found = get_number_of_flags(*flags);
  while ((i < count)) {
    int index = *(file_indexes + i);
    work_with_file(argv[index], error, flags, flags_found);
    i++;
  }
}
