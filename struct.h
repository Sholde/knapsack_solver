#ifndef _STRUCT_H_
#define _STRUCT_H_

// Personal type
typedef char               byte;
typedef unsigned long long u64;

// single variable firstn then array to optimize memory locality
typedef struct knapsack_s
{
  // Implementation
  u64 index;
  u64 size_2;

  // Standart
  double max_weight;

  u64 size;
  double *value;
  double *weight;

  // Implementation
  double *value_by_weight;
  double *max_value;
  byte *taken;
} knapsack_t;

// Constant
#define CSV_LINE_SIZE 128

// Macro
#define max(a, b) (a < b ? b : a)
#define power2(a) (1 << a)

#endif // _STRUCT_H_
