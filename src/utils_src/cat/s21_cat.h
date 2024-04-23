#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_
#include <stdio.h>

#include "parser.h"

int is_Tab_char(unsigned char cur_char);
int is_NEL_char(unsigned char cur_char);
void handle_unprintable(unsigned char cur_char);
void print_new_line(Flags *flags);
void print_with_flags(const unsigned char *buffer, int len, Flags *flags,
                      int *empty_repeat);
int get_number_of_flags(Flags flags);
int read_line(unsigned char *buffer, FILE *source, int *len);
void s21_cat(FILE *source, int with_flags, Flags *flags);
void find_files(char *argv[], Flags *flags, int *file_indexes, int count,
                Errors *error);
void work_with_file(char *filename, Errors *error, Flags *flags,
                    int flags_found);
int parse_args(int argc, char *argv[], Flags *flags, Errors *error);

#endif  // SRC_CAT_S21_CAT_H_
