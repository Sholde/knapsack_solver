#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long ull;

typedef struct knapsack_s
{
  ull size;
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

ull count_number_of_line(FILE *f)
{
  ull size = 0;
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

void print_knapsack(knapsack_t *ks)
{
  fprintf(stderr, "\"value\", \"weight\"\n");
  for (int i = 0; i < ks->size; i++)
    {
      fprintf(stderr, "%f, %f\n", ks->value[i], ks->weight[i]);
    }
}

void solve_knapsack(knapsack_t *ks)
{
  
}

int main(int argc, char **argv)
{
  if (argc != 2)
    return printf("Error: needed the path of csv file in argument.\n"), 1;

  knapsack_t *ks = parse_csv_file(argv[1]);
  print_knapsack(ks);
  solve_knapsack(ks);
  free_knapsack(ks);
  
  return 0;
}
