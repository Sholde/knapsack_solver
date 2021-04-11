#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "struct.h"
#include "display.h"
#include "knapsack.h"

const char *getfield(char *line, int num)
{
  const char *tok;

  for (tok = strtok(line, ", "); tok && *tok; tok = strtok(NULL, ", \n"))
    {
      if (!--num)
        return tok;
    }

  return NULL;  
}

u64 count_number_of_line(FILE *f)
{
  u64 size = 0;
  char buff[CSV_LINE_SIZE];
  
  fseek(f, 0, SEEK_SET);
  
  while (fgets(buff, CSV_LINE_SIZE, f))
    {
      size++;
    }
  
  fseek(f, 0, SEEK_SET);
  
  return size;
}

void parse_csv_file(knapsack_t *ks, char *path)
{
  // Open the file
  FILE *f = fopen(path, "r");

  if (!f)
    {
      perror("fopen");
      free_knapsack(ks);
      exit(EXIT_FAILURE);
    }

  // Check if file is empty
  fseek(f, 0, SEEK_END);
  int is_empty = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (is_empty == 0)
    {
      fprintf(stderr, "Error: file is empty!\n");
      free_knapsack(ks);
      exit(EXIT_FAILURE);
    }

  // Count the number of line
  ks->size = count_number_of_line(f) - 1;

  // Allocate memory
  ks->value = malloc(sizeof(double) * ks->size);
  ks->weight = malloc(sizeof(double) * ks->size);

  // Buffer
  char line[CSV_LINE_SIZE];
  char tmp[CSV_LINE_SIZE];

  // Check csv format
  char *ret = fgets(line, CSV_LINE_SIZE, f);

  if (!ret)
    {
      perror("fgets");
      free_knapsack(ks);
      exit(EXIT_FAILURE);
    }

  strcpy(tmp, line);
  const char *value_str = getfield(tmp, 1);

  strcpy(tmp, line);
  const char *weight_str = getfield(tmp, 2);

  if (strcmp(value_str, "\"value\"") != 0 || strcmp(weight_str, "\"weight\"") != 0)
    {
      fprintf(stderr, "Error: format of csv file must be \"value\", \"weight\"\n");
      free_knapsack(ks);
      exit(EXIT_FAILURE);
    }

  // Fill arrays
  int i = 0;
  while (i < ks->size && fgets(line, CSV_LINE_SIZE, f))
    {
      // Fill value
      strcpy(tmp, line);
      value_str = getfield(tmp, 1);
      ks->value[i] = strtod(value_str, NULL);

      // Fill weight
      strcpy(tmp, line);
      weight_str = getfield(tmp, 2);
      ks->weight[i] = strtod(weight_str, NULL);

      // Increment index
      i++;
    }

  fclose(f);
}

void print_error(char *bin)
{
  fprintf(stderr, "Usage: %s [OPTION] [FILE]...\n", bin);
  fprintf(stderr, "try %s --help to get more information.\n", bin);
}

void print_help(char *bin)
{
  fprintf(stderr, "Usage: %s [OPTION] [FILE] [MAX WEIGHT]\n", bin);
  fprintf(stderr, "  -h, --help     print this help\n");
  fprintf(stderr, "  -i, --input    must be csv file with 2 column: \"value\", \"weight\"." \
          " Use this csv file to init the knapsack solver.\n");
}

void print_nspace(u64 space)
{
  for (int i = 0; i < space; i++)
    {
      fprintf(stderr, " ");
    }
}

void print_knapsack(knapsack_t *ks)
{
  // Problem
  fprintf(stderr, "Knapsack problem input:\n");

  fprintf(stderr, " - max weight: %f\n", ks->max_weight);

  fprintf(stderr, " - file input:\n");
  fprintf(stderr, "     \"value\", \"weight\"\n");

  for (int i = 0; i < ks->size; i++)
    {
      fprintf(stderr, "     %f, %f\n", ks->value[i], ks->weight[i]);
    }

  // Separate
  fprintf(stderr, "\n");

  // Result
  fprintf(stderr, "Result:\n");

  double result = compute_value_knapsack(ks);
  fprintf(stderr, " - max value: %f\n", result);

  fprintf(stderr, " - list of pair taken:\n");
  fprintf(stderr, "     \"value\", \"weight\"\n");

  for (int i = 0; i < ks->size; i++)
    {
      if (ks->taken[i])
        fprintf(stderr, "     %f, %f\n", ks->value[i], ks->weight[i]);
    }

  fprintf(stderr, " - tree:\n");

  u64 space = 30;
  u64 shift = 3;

  for (int i = 0; i < ks->size; i++)
    {
      print_nspace(space);
      fprintf(stderr, "%.1f, %.1f\n", ks->value[i], ks->weight[i]);

      if (ks->taken[i])
        {
          space -= 2;
          print_nspace(space + shift);
          fprintf(stderr, "/\n");
          space -= 1;
        }
      else
        {
          space += 2;
          print_nspace(space + shift);
          fprintf(stderr, "\\\n");
          space += 1;
        }
    }

  print_nspace(space + shift);
  fprintf(stderr, "END\n");
}
