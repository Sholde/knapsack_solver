#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "struct.h"
#include "display.h"
#include "knapsack.h"

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
      // Check if we have 3 argument
      if (argc != 4)
        return print_error(argv[0]), 1;

      // Initialisation
      knapsack_t *ks = init_knapsack(argv[2], strtod(argv[3], NULL));

      //
      double elapsed;
      struct timespec before, after;

      // Measure time
      clock_gettime(CLOCK_MONOTONIC_RAW, &before);
      {
        sort_knapsack(ks);
        solve_knapsack(ks, 0, 0.0);
        search_knapsack(ks);
      }
      clock_gettime(CLOCK_MONOTONIC_RAW, &after);

      // Print knapsack
      print_knapsack(ks);

      // Compute alapsed time
      elapsed = after.tv_nsec - before.tv_nsec;

      // Check if measure is correct
      if (elapsed < 0)
        {
          fprintf(stderr, "Time in second(s): non-determinated\n");
        }
      else
        {
          // nano-seconds to seconds
          elapsed = elapsed / 1.0e9;

          fprintf(stderr, "Time in second(s): %f\n", elapsed);
        }

      // Free knapsack
      free_knapsack(ks);
    }
  else
    {
      return print_error(argv[0]), 1;
    }
  
  return 0;
}
