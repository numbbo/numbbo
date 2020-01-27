#include <stdlib.h>
#include <stdio.h>

/**
 * An evaluator for problems from the toy-socket suite to demonstrate external evaluation in C
 */
void evaluate_toy_socket(char *suite_name, size_t number_of_objectives, size_t function,
    size_t instance, size_t dimension, const double *x, double *y)
{
  double value;
  size_t i;

  if ((strcmp(suite_name, "toy-socket") == 0) && (number_of_objectives == 1)) {
    value = (int)instance * 100.0;
    if (function == 1) {
      /* Function 1 is the sum of all values */
      for (i = 0; i < dimension; i++) {
        value += x[i];
      }
      y[0] = value;
    } else if (function == 2) {
      /* Function 2 is the sum of squares of all values */
      for (i = 0; i < dimension; i++) {
        value += x[i] * x[i];
      }
      y[0] = value;
    } else {
      fprintf(stderr, "evaluate(): suite %s has no function %lu", suite_name, function);
      exit(EXIT_FAILURE);
    }
  } else if ((strcmp(suite_name, "toy-socket-biobj") == 0) && (number_of_objectives == 2)) {
    if (function == 1) {
      /* Objective 1 is the sum of all values */
      value = (int)instance * 100.0;
      for (i = 0; i < dimension; i++) {
        value += x[i];
      }
      y[0] = value;
      /* Objective 2 is the sum of squares of all values */
      value = (int)instance * 100.0;
      for (i = 0; i < dimension; i++) {
        value += x[i] * x[i];
      }
      y[1] = value;
    } else {
      fprintf(stderr, "evaluate(): suite %s has no function %lu", suite_name, function);
      exit(EXIT_FAILURE);
    }

  } else {
    fprintf(stderr, "evaluate(): suite %s cannot have %lu objectives",
        suite_name, number_of_objectives);
    exit(EXIT_FAILURE);
  }

}
