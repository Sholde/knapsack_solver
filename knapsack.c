#include <stdlib.h>

#include "struct.h"
#include "knapsack.h"

knapsack_t *init_knapsack(char *path, double max_weight)
{
  // Allocate structure
  knapsack_t *ks = malloc(sizeof(knapsack_t));

  // Default init
  ks->index = 0;
  ks->size = 0;
  ks->size_2 = 0;
  ks->value = NULL;
  ks->weight = NULL;
  ks->value_by_weight = NULL;
  ks->max_value = NULL;
  ks->taken = NULL;

  // Parsing file
  parse_csv_file(ks, path);

  // Specific init
  ks->size_2 = power2(ks->size);
  ks->max_weight = max_weight;

  // Allocate the remain
  ks->value_by_weight = malloc(sizeof(double) * ks->size);
  ks->max_value = malloc(sizeof(double) * ks->size_2);
  ks->taken = malloc(sizeof(byte) * ks->size);

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
  // Bufferize
  double tmp_value = ks->value[first];
  double tmp_weight = ks->weight[first];
  double tmp_vbw = ks->value_by_weight[first];

  // Put the second in the first
  ks->value[first] = ks->value[second];
  ks->weight[first] = ks->weight[second];
  ks->value_by_weight[first] = ks->value_by_weight[second];

  // Put the first in the second
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
  // After leaves
  if (index >= ks->size)
    {
      if (effective_weight < 0)
        ks->max_value[ks->index] = -1;
      else
        ks->max_value[ks->index] = compute_value_knapsack(ks);
      
      ks->index++;
      return;
    }

  // We are full
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
  else // take index but we are full
    {
      ks->taken[index] = 1;
      solve_knapsack(ks, index + 1, -1);
    }


  // Don't take index
  ks->taken[index] = 0;
  solve_knapsack(ks, index + 1, effective_weight);
}

void search_knapsack(knapsack_t *ks)
{
  // Search the index of the max value
  u64 max_index = 0;

  for (int i = 0; i < ks->size_2 - 1; i++)
    {
      if (ks->max_value[max_index] < ks->max_value[i])
        {
          max_index = i;
        }
    }

  // Reconstitute the main path
  // We begin from the leaves
  for (int i = ks->size - 1; i >= 0; i--)
    {
      // Left is taken and right is not taken
      // So at left we have power of 2, and right not
      // Thus we have taken if it is left (or power of 2)
      if (max_index % 2 == 0)
        {
          ks->taken[i] = 1;
        }
      else
        {
          ks->taken[i] = 0;
        }

      // Divide by 2 because we go up on tree
      // (and each upstair have 2 times less nodes because we have binary tree)
      max_index = max_index / 2;
    }
}
