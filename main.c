#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char               byte;
typedef unsigned long long u64;

typedef struct knapsack_s
{
  u64 index;
  u64 size_2;
  
  double max_weight;

  u64 size;
  double *value;
  double *weight;

  double *value_by_weight;
  double *max_value;
  byte *taken;
} knapsack_t;

#define CSV_LINE_SIZE 128
#define max(a, b) (a < b ? b : a)
#define power2(a) (1 << a)

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

void free_knapsack(knapsack_t *ks);

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

knapsack_t *init_knapsack(char *path, double max_weight)
{
  knapsack_t *ks = malloc(sizeof(knapsack_t));
  ks->size = 0;
  ks->value = NULL;
  ks->weight = NULL;
  ks->value_by_weight = NULL;
  ks->taken = NULL;

  parse_csv_file(ks, path);

  ks->size_2 = power2(ks->size);

  ks->max_weight = max_weight;
  ks->value_by_weight = malloc(sizeof(double) * ks->size);
  ks->max_value = malloc(sizeof(double) * ks->size_2);
  ks->taken = malloc(sizeof(byte) * ks->size);

  ks->index = 0;

  return ks;
}

void free_knapsack(knapsack_t *ks)
{
  if (ks)
    {
      if (ks->value)
        free(ks->value);
      
      if (ks->weight)
        free(ks->weight);

      if (ks->value_by_weight)
        free(ks->value_by_weight);

      if (ks->max_value)
        free(ks->max_value);

      if (ks->taken)
        free(ks->taken);

      free(ks);
    }
}

void exchange_knapsack(knapsack_t *ks, u64 first, u64 second)
{
  double tmp_value = ks->value[first];
  double tmp_weight = ks->weight[first];
  double tmp_vbw = ks->value_by_weight[first];

  ks->value[first] = ks->value[second];
  ks->weight[first] = ks->weight[second];
  ks->value_by_weight[first] = ks->value_by_weight[second];

  ks->value[second] = tmp_value;
  ks->weight[second] = tmp_weight;
  ks->value_by_weight[second] = tmp_vbw;
}

void sort_knapsack(knapsack_t *ks)
{
  // Computing
  for (int i = 0; i < ks->size; i++)
    {
      ks->value_by_weight[i] = ks->value[i] / ks->weight[i];
    }

  u64 index = 0;

  // Sorting (The most simple algorithm)
  for (u64 i = 0; i < ks->size; i++)
    {
      // Init to i
      index = i;

      // Search the maximum value by weight after i
      for (u64 j = i + 1; j < ks->size; j++)
        {
          if (ks->value_by_weight[index] < ks->value_by_weight[j])
            {
              index = j;
            }
        }

      // If we have we exchange it position with i
      if (i != index)
        {
          exchange_knapsack(ks, i, index);
        }
    }
}

double compute_value_knapsack(knapsack_t *ks)
{
  double value = 0.0;

  for (int i = 0; i < ks->size; i++)
    {
      if (ks->taken[i])
        {
          value += ks->value[i];
        }
    }

  return value;
}

void solve_knapsack(knapsack_t *ks, u64 index, double effective_weight)
{
  if (index >= ks->size)
    {
      if (effective_weight < 0)
        ks->max_value[ks->index] = -1;
      else
        ks->max_value[ks->index] = compute_value_knapsack(ks);
      
      ks->index++;
      return;
    }

  if (effective_weight < 0)
    {
      ks->taken[index] = 0;
      solve_knapsack(ks, index + 1, -1);
      solve_knapsack(ks, index + 1, -1);
      return;
    }

  // Take index
  if (effective_weight + ks->weight[index] <= ks->max_weight)
    {
      ks->taken[index] = 1;
      solve_knapsack(ks, index + 1, effective_weight + ks->weight[index]);
    }
  else
    {
      ks->taken[index] = 1;
      solve_knapsack(ks, index + 1, -1);
    }


  // Don't take index
  ks->taken[index] = 0;
  solve_knapsack(ks, index + 1, effective_weight);
}

void search(knapsack_t *ks)
{
  u64 max_index = 0;
  for (int i = 0; i < ks->size_2 - 1; i++)
    {
      if (ks->max_value[max_index] < ks->max_value[i])
        {
          max_index = i;
        }
    }

  for (int i = ks->size - 1; i >= 0; i--)
    {
      if (max_index % 2 == 0)
        {

          ks->taken[i] = 1;
        }
      else
        {
          ks->taken[i] = 0;
        }

      max_index = max_index / 2;
    }
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

  fprintf(stderr, " - list of value taken:\n");
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
  fprintf(stderr, "FIN\n");
}

int main(int argc, char **argv)
{
  if (argc < 2)
    return print_error(argv[0]), 1;

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
      print_help(argv[0]);
    }
  else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--input") == 0)
    {
      if (argc != 4)
        return print_error(argv[0]), 1;

      knapsack_t *ks = init_knapsack(argv[2], strtod(argv[3], NULL));

      sort_knapsack(ks);
      solve_knapsack(ks, 0, 0.0);

      search(ks);
      
      print_knapsack(ks);

      free_knapsack(ks);
    }
  else
    {
      return print_error(argv[0]), 1;
    }
  
  return 0;
}
