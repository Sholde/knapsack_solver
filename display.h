#ifndef _DISPLAY_H_
#define _DISPLAY_H_

const char *getfield(char *line, int num);
u64 count_number_of_line(FILE *f);
void parse_csv_file(knapsack_t *ks, char *path);
void print_error(char *bin);
void print_help(char *bin);
void print_nspace(u64 space);
void print_knapsack(knapsack_t *ks);

#endif // _DISPLAY_H_
