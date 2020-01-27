/**
 * @file suite_toy_socket.c
 *
 * @brief Implementation of a single-objective suite containing two problems to show the
 * functioning of socket communication for function evaluation. A bi-objective version can be
 * found in the file suite_toy_socket_biobj.c
 *
 * The suite contains 2 problems with dimensions 2, 30 and 1 instance.
 */

#include "coco.h"
#include "toy_socket.c"
#include "socket_communication.c"

static coco_suite_t *coco_suite_allocate(const char *suite_name,
                                         const size_t number_of_functions,
                                         const size_t number_of_dimensions,
                                         const size_t *dimensions,
                                         const char *default_instances);


/**
 * @brief Sets the dimensions and default instances for the toy-socket suite.
 * Sets also the parameters needed for socket communication with the external evaluator.
 */
static coco_suite_t *suite_toy_socket_initialize(const char *suite_options) {

  coco_suite_t *suite;
  const size_t dimensions[] = { 2, 30 };

  suite = coco_suite_allocate("toy-socket", 2, 2, dimensions, "instances: 1");

  suite->data = socket_communication_data_initialize(suite_options);
  suite->data_free_function = socket_communication_data_free;
  return suite;
}

/**
 * @brief Sets the instances associated with years.
 */
static const char *suite_toy_socket_get_instances_by_year(const int year) {
   if (year == 0) {
    return "1";
  }
  else {
    coco_error("suite_toy_socket_get_instances_by_year(): year %d not defined for suite toy-socket", year);
    return NULL;
  }
}

/**
 * @brief Returns the problem from the toy-socket suite that corresponds to the given parameters.
 *
 * @param suite The COCO suite.
 * @param function_idx Index of the function (starting from 0).
 * @param dimension_idx Index of the dimension (starting from 0).
 * @param instance_idx Index of the instance (starting from 0).
 * @return The problem that corresponds to the given parameters.
 */
static coco_problem_t *suite_toy_socket_get_problem(coco_suite_t *suite,
                                                    const size_t function_idx,
                                                    const size_t dimension_idx,
                                                    const size_t instance_idx) {

  coco_problem_t *problem = NULL;

  const size_t function = suite->functions[function_idx];
  const size_t dimension = suite->dimensions[dimension_idx];
  const size_t instance = suite->instances[instance_idx];

  const char *problem_id_template = "toy_socket_f%02lu_i%02lu_d%02lu";
  const char *problem_name_template = "single-objective toy socket suite problem f%lu instance %lu in %luD";

  problem = toy_socket_problem_allocate(1, function, dimension, instance,
      problem_id_template, problem_name_template);
  assert(problem != NULL);

  problem->suite_dep_function = function;
  problem->suite_dep_instance = instance;
  problem->suite_dep_index = coco_suite_encode_problem_index(suite, function_idx, dimension_idx, instance_idx);
  return problem;
}
