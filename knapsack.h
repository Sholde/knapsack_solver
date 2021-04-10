#ifndef _KNAPSACK_H_
#define _KNAPSACK_H_

knapsack_t *init_knapsack(char *path, double max_weight);
void free_knapsack(knapsack_t *ks);
void exchange_knapsack(knapsack_t *ks, u64 first, u64 second);
void sort_knapsack(knapsack_t *ks);
double compute_value_knapsack(knapsack_t *ks);
void solve_knapsack(knapsack_t *ks, u64 index, double effective_weight);
void search_knapsack(knapsack_t *ks);

#endif // _KNAPSACK_H_
