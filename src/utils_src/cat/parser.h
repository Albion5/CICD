#ifndef SRC_CAT_PARSER_H_
#define SRC_CAT_PARSER_H_

typedef struct Flags {
  int number_all;
  int number_non_empty;
  int show_tab;
  int show_end;
  int show_unprintable;
  int squeeze;
  int count;
} Flags;

typedef struct Errors {
  int error_code;
  char symbol;
  char string[200];
} Errors;

void print_long_flag_error(char *option);
void print_single_flag_error(char option);
void print_file_error(char *filename);
void print_error(Errors error);
void set_no_error(Errors *error);
void set_sflag_error(Errors *error, char option);
void set_lflag_error(Errors *error, char *option);
void set_file_error(Errors *error, char *filename);
void set_b(Flags *flags);
void set_e(Flags *flags);
void set_E(Flags *flags);
void set_v(Flags *flags);
void set_n(Flags *flags);
void set_s(Flags *flags);
void set_t(Flags *flags);
void set_T(Flags *flags);
int check_single_flag(char symbol, Flags *flags, Errors *error);
int check_long_flag(char *string, Flags *flags, Errors *error);
int check_flag(char *string, Flags *flags, Errors *error);
void find_flags(int argc, char *argv[], Flags *flags, int *file_indexes,
                int *count, Errors *error);

#endif  // SRC_CAT_PARSER_H_
