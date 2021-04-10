#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long u64;

typedef struct knapsack_s
{
  u64 max_weight;
  u64 size;
  double *value;
  double *weight;
} knapsack_t;

#define CSV_LINE_SIZE 128

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

knapsack_t *parse_csv_file(char *path)
{
  knapsack_t *ks = malloc(sizeof(knapsack_t));
  ks->size = 0;
  ks->value = NULL;
  ks->weight = NULL;

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

      free(ks);
    }
}

void exchange_knapsack(knapsack_t *ks, double *value_by_weight, u64 first, u64 second)
{
  double tmp_value = ks->value[first];
  double tmp_weight = ks->weight[first];
  double tmp_vbw = value_by_weight[first];

  ks->value[first] = ks->value[second];
  ks->weight[first] = ks->weight[second];
  value_by_weight[first] = value_by_weight[second];

  ks->value[second] = tmp_value;
  ks->weight[second] = tmp_weight;
  value_by_weight[second] = tmp_vbw;
}

void print_knapsack(knapsack_t *ks)
{
  fprintf(stderr, "\"value\", \"weight\"\n");
  for (int i = 0; i < ks->size; i++)
    {
      fprintf(stderr, "%f, %f\n", ks->value[i], ks->weight[i]);
    }
}

void sort_knapsack(knapsack_t *ks)
{
  double *value_by_weight = malloc(sizeof(double) * ks->size);

  // Computing
  for (int i = 0; i < ks->size; i++)
    {
      value_by_weight[i] = ks->value[i] / ks->weight[i];
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
          if (value_by_weight[index] < value_by_weight[j])
            {
              index = j;
            }
        }

      // If we have we exchange it position with i
      if (i != index)
        {
          exchange_knapsack(ks, value_by_weight, i, index);
        }
    }

  free(value_by_weight);
}

void solve_knapsack(knapsack_t *ks)
{
  sort_knapsack(ks);
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

      knapsack_t *ks = parse_csv_file(argv[2]);

      ks->max_weight = atoll(argv[3]);

      solve_knapsack(ks);
      print_knapsack(ks);

      free_knapsack(ks);
    }
  else
    {
      return print_error(argv[0]), 1;
    }
  
  return 0;
}
