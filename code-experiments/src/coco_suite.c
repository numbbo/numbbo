/**
 * @file coco_suite.c
 * @brief Definitions of functions regarding COCO suites.
 *
 * When a new suite is added, the functions coco_suite_intialize, coco_suite_get_instances_by_year and
 * coco_suite_get_problem_from_indices need to be updated.
 *
 * @see <a href="index.html">Instructions</a> on how to write new test functions and combine them into test
 * suites.
 */

#include <time.h>

#include "coco.h"
#include "coco_internal.h"
#include "coco_utilities.c"

#include "suite_bbob.c"
#include "suite_bbob_mixint.c"
#include "suite_biobj.c"
#include "suite_biobj_mixint.c"
#include "suite_toy.c"
#include "suite_largescale.c"
#include "suite_cons_bbob.c"
#include "suite_toy_socket.c"
#include "suite_toy_socket_biobj.c"

/** @brief The maximum number of different instances in a suite. */
#define COCO_MAX_INSTANCES 1000

/**
 * @brief Calls the initializer of the given suite.
 *
 * @note This function needs to be updated when a new suite is added to COCO.
 */
static coco_suite_t *coco_suite_intialize(const char *suite_name, const char *suite_options) {

  coco_suite_t *suite;

  if (strcmp(suite_name, "toy") == 0) {
    suite = suite_toy_initialize();
  } else if (strcmp(suite_name, "bbob") == 0) {
    suite = suite_bbob_initialize();
  } else if ((strcmp(suite_name, "bbob-biobj") == 0) ||
      (strcmp(suite_name, "bbob-biobj-ext") == 0)) {
    suite = suite_biobj_initialize(suite_name);
  } else if (strcmp(suite_name, "bbob-largescale") == 0) {
    suite = suite_largescale_initialize();
  } else if (strcmp(suite_name, "bbob-constrained") == 0) {
    suite = suite_cons_bbob_initialize();
  } else if (strcmp(suite_name, "bbob-mixint") == 0) {
    suite = suite_bbob_mixint_initialize(suite_name);
  } else if (strcmp(suite_name, "bbob-biobj-mixint") == 0) {
    suite = suite_biobj_mixint_initialize();
  } else if (strcmp(suite_name, "toy-socket") == 0) {
    suite = suite_toy_socket_initialize(suite_options);
  } else if (strcmp(suite_name, "toy-socket-biobj") == 0) {
    suite = suite_toy_socket_biobj_initialize(suite_options);
  }
  else {
    coco_error("coco_suite_intialize(): unknown problem suite");
    return NULL;
  }

  return suite;
}

/**
 * @brief Calls the function that sets the instanced by year for the given suite.
 *
 * @note This function needs to be updated when a new suite is added to COCO.
 */
static const char *coco_suite_get_instances_by_year(const coco_suite_t *suite, const int year) {
  const char *year_string;

  if (strcmp(suite->suite_name, "bbob") == 0) {
    year_string = suite_bbob_get_instances_by_year(year);
  } else if (strcmp(suite->suite_name, "bbob-constrained") == 0) {
    year_string = suite_cons_bbob_get_instances_by_year(year);
  } else if ((strcmp(suite->suite_name, "bbob-biobj") == 0) ||
      (strcmp(suite->suite_name, "bbob-biobj-ext") == 0)) {
    year_string = suite_biobj_get_instances_by_year(year);
  } else if (strcmp(suite->suite_name, "bbob-largescale") == 0) {
    year_string = suite_largescale_get_instances_by_year(year);
  } else if (strcmp(suite->suite_name, "bbob-mixint") == 0) {
    year_string = suite_bbob_mixint_get_instances_by_year(year);
  } else if (strcmp(suite->suite_name, "bbob-biobj-mixint") == 0) {
    year_string = suite_biobj_mixint_get_instances_by_year(year);
  } else if (strcmp(suite->suite_name, "toy-socket") == 0) {
    year_string = suite_toy_socket_get_instances_by_year(year);
  } else if (strcmp(suite->suite_name, "toy-socket-biobj") == 0) {
    year_string = suite_toy_socket_biobj_get_instances_by_year(year);
  } else {
    coco_error("coco_suite_get_instances_by_year(): suite '%s' has no years defined", suite->suite_name);
    return NULL;
  }

  return year_string;
}

/**
 * @brief Calls the function that returns the problem corresponding to the given suite, function index,
 * dimension index and instance index. If the indices don't correspond to a problem because of suite
 * filtering, it returns NULL.
 *
 * @note This function needs to be updated when a new suite is added to COCO.
 */
static coco_problem_t *coco_suite_get_problem_from_indices(coco_suite_t *suite,
                                                           const size_t function_idx,
                                                           const size_t dimension_idx,
                                                           const size_t instance_idx) {

  coco_problem_t *problem;
  
  if ((suite->functions[function_idx] == 0) ||
      (suite->dimensions[dimension_idx] == 0) ||
    (suite->instances[instance_idx] == 0)) {
    return NULL;
  }

  if (strcmp(suite->suite_name, "toy") == 0) {
    problem = suite_toy_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if (strcmp(suite->suite_name, "bbob") == 0) {
    problem = suite_bbob_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if ((strcmp(suite->suite_name, "bbob-biobj") == 0) ||
      (strcmp(suite->suite_name, "bbob-biobj-ext") == 0)) {
    problem = suite_biobj_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if (strcmp(suite->suite_name, "bbob-largescale") == 0) {
    problem = suite_largescale_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if (strcmp(suite->suite_name, "bbob-constrained") == 0) {
    problem = suite_cons_bbob_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if (strcmp(suite->suite_name, "bbob-mixint") == 0) {
    problem = suite_bbob_mixint_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if (strcmp(suite->suite_name, "bbob-biobj-mixint") == 0) {
    problem = suite_biobj_mixint_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if (strcmp(suite->suite_name, "toy-socket") == 0) {
    problem = suite_toy_socket_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else if (strcmp(suite->suite_name, "toy-socket-biobj") == 0) {
    problem = suite_toy_socket_biobj_get_problem(suite, function_idx, dimension_idx, instance_idx);
  } else {
    coco_error("coco_suite_get_problem_from_indices(): unknown problem suite");
    return NULL;
  }

  coco_problem_set_suite(problem, suite);

  return problem;
}

/**
 * @brief Returns the best indicator value for the given problem.
 */
static double coco_suite_get_best_indicator_value(const coco_suite_t *suite,
                                                  const coco_problem_t *problem,
                                                  const char *indicator_name) {
  double result = 0;

  if (strcmp(indicator_name, "hyp") == 0) {
    result = suite_biobj_get_best_hyp_value(suite->suite_name, problem->problem_id);
  } else {
    coco_error("coco_suite_get_best_indicator_value(): indicator %s not supported", indicator_name);
    return 0; /* Never reached */
  }
  return result;
}

/**
 * @note: While a suite can contain multiple problems with equal function, dimension and instance, this
 * function always returns the first problem in the suite with the given function, dimension and instance
 * values. If the given values don't correspond to a problem, the function returns NULL.
 */
coco_problem_t *coco_suite_get_problem_by_function_dimension_instance(coco_suite_t *suite,
                                                                      const size_t function,
                                                                      const size_t dimension,
                                                                      const size_t instance) {

  size_t i;
  int function_idx, dimension_idx, instance_idx;
  int found;

  found = 0;
  for (i = 0; i < suite->number_of_functions; i++) {
    if (suite->functions[i] == function) {
      function_idx = (int) i;
      found = 1;
      break;
    }
  }
  if (!found)
    return NULL;

  found = 0;
  for (i = 0; i < suite->number_of_dimensions; i++) {
    if (suite->dimensions[i] == dimension) {
      dimension_idx = (int) i;
      found = 1;
      break;
    }
  }
  if (!found)
    return NULL;

  found = 0;
  for (i = 0; i < suite->number_of_instances; i++) {
    if (suite->instances[i] == instance) {
      instance_idx = (int) i;
      found = 1;
      break;
    }
  }
  if (!found)
    return NULL;

  return coco_suite_get_problem_from_indices(suite, (size_t) function_idx, (size_t) dimension_idx, (size_t) instance_idx);
}


/**
 * @brief Allocates the space for a coco_suite_t instance.
 *
 * This function sets the functions and dimensions contained in the suite, while the instances are set by
 * the function coco_suite_set_instance.
 */
static coco_suite_t *coco_suite_allocate(const char *suite_name,
                                         const size_t number_of_functions,
                                         const size_t number_of_dimensions,
                                         const size_t *dimensions,
                                         const char *default_instances) {

  coco_suite_t *suite;
  size_t i;

  suite = (coco_suite_t *) coco_allocate_memory(sizeof(*suite));

  suite->suite_name = coco_strdup(suite_name);

  suite->number_of_dimensions = number_of_dimensions;
  assert(number_of_dimensions > 0);
  suite->dimensions = coco_allocate_vector_size_t(suite->number_of_dimensions);
  for (i = 0; i < suite->number_of_dimensions; i++) {
    suite->dimensions[i] = dimensions[i];
  }

  suite->number_of_functions = number_of_functions;
  assert(number_of_functions > 0);
  suite->functions = coco_allocate_vector_size_t(suite->number_of_functions);
  for (i = 0; i < suite->number_of_functions; i++) {
    suite->functions[i] = i + 1;
  }

  assert(strlen(default_instances) > 0);
  suite->default_instances = coco_strdup(default_instances);

  /* Will be set to the first valid dimension index before the constructor ends */
  suite->current_dimension_idx = -1;
  /* Will be set to the first valid function index before the constructor ends  */
  suite->current_function_idx = -1;

  suite->current_instance_idx = -1;
  suite->current_problem = NULL;

  /* To be set in coco_suite_set_instance() */
  suite->number_of_instances = 0;
  suite->instances = NULL;

  /* To be set in particular suites if needed */
  suite->data = NULL;
  suite->data_free_function = NULL;

  return suite;
}

/**
 * @brief Sets the suite instance to the given instance_numbers.
 */
static void coco_suite_set_instance(coco_suite_t *suite,
                                    const size_t *instance_numbers) {

  size_t i;

  if (!instance_numbers) {
    coco_error("coco_suite_set_instance(): no instance given");
    return;
  }

  suite->number_of_instances = coco_count_numbers(instance_numbers, COCO_MAX_INSTANCES, "suite instance numbers");
  suite->instances = coco_allocate_vector_size_t(suite->number_of_instances);
  for (i = 0; i < suite->number_of_instances; i++) {
    suite->instances[i] = instance_numbers[i];
  }

}

/**
 * @brief Filters the given items w.r.t. the given indices (starting from 1).
 *
 * Sets items[i - 1] to 0 for every i that cannot be found in indices (this function performs the conversion
 * from user-friendly indices starting from 1 to C-friendly indices starting from 0).
 */
static void coco_suite_filter_indices(size_t *items, const size_t number_of_items, const size_t *indices, const char *name) {

  size_t i, j;
  size_t count = coco_count_numbers(indices, COCO_MAX_INSTANCES, name);
  int found;

  for (i = 1; i <= number_of_items; i++) {
    found = 0;
    for (j = 0; j < count; j++) {
      if (i == indices[j]) {
        found = 1;
        break;
      }
    }
    if (!found)
      items[i - 1] = 0;
  }

}

/**
 * @brief Filters dimensions w.r.t. the given dimension_numbers.
 *
 * Sets suite->dimensions[i] to 0 for every dimension value that cannot be found in dimension_numbers.
 */
static void coco_suite_filter_dimensions(coco_suite_t *suite, const size_t *dimension_numbers) {

  size_t i, j;
  size_t count = coco_count_numbers(dimension_numbers, COCO_MAX_INSTANCES, "dimensions");
  int found;

  for (i = 0; i < suite->number_of_dimensions; i++) {
    found = 0;
    for (j = 0; j < count; j++) {
      if (suite->dimensions[i] == dimension_numbers[j])
        found = 1;
    }
    if (!found)
      suite->dimensions[i] = 0;
  }

}

/**
 * @param suite The given suite.
 * @param function_idx The index of the function in question (starting from 0).
 *
 * @return The function number in position function_idx in the suite. If the function has been filtered out
 * through suite_options in the coco_suite function, the result is 0.
 */
size_t coco_suite_get_function_from_function_index(const coco_suite_t *suite, const size_t function_idx) {

  if (function_idx >= suite->number_of_functions) {
    coco_error("coco_suite_get_function_from_function_index(): function index exceeding the number of functions in the suite");
    return 0; /* Never reached*/
  }

 return suite->functions[function_idx];
}

/**
 * @param suite The given suite.
 * @param dimension_idx The index of the dimension in question (starting from 0).
 *
 * @return The dimension number in position dimension_idx in the suite. If the dimension has been filtered out
 * through suite_options in the coco_suite function, the result is 0.
 */
size_t coco_suite_get_dimension_from_dimension_index(const coco_suite_t *suite, const size_t dimension_idx) {

  if (dimension_idx >= suite->number_of_dimensions) {
    coco_error("coco_suite_get_dimension_from_dimension_index(): dimensions index exceeding the number of dimensions in the suite");
    return 0; /* Never reached*/
  }

 return suite->dimensions[dimension_idx];
}

/**
 * @param suite The given suite.
 * @param instance_idx The index of the instance in question (starting from 0).
 *
 * @return The instance number in position instance_idx in the suite. If the instance has been filtered out
 * through suite_options in the coco_suite function, the result is 0.
 */
size_t coco_suite_get_instance_from_instance_index(const coco_suite_t *suite, const size_t instance_idx) {

  if (instance_idx >= suite->number_of_instances) {
    coco_error("coco_suite_get_instance_from_instance_index(): instance index exceeding the number of instances in the suite");
    return 0; /* Never reached*/
  }

 return suite->functions[instance_idx];
}

void coco_suite_free(coco_suite_t *suite) {

  if (suite != NULL) {

    if (suite->suite_name) {
      coco_free_memory(suite->suite_name);
      suite->suite_name = NULL;
    }
    if (suite->dimensions) {
      coco_free_memory(suite->dimensions);
      suite->dimensions = NULL;
    }
    if (suite->functions) {
      coco_free_memory(suite->functions);
      suite->functions = NULL;
    }
    if (suite->instances) {
      coco_free_memory(suite->instances);
      suite->instances = NULL;
    }
    if (suite->default_instances) {
      coco_free_memory(suite->default_instances);
      suite->default_instances = NULL;
    }

    if (suite->current_problem) {
      coco_problem_free(suite->current_problem);
      suite->current_problem = NULL;
    }

    if (suite->data != NULL) {
      if (suite->data_free_function != NULL) {
        suite->data_free_function(suite->data);
      }
      coco_free_memory(suite->data);
      suite->data = NULL;
    }

    coco_free_memory(suite);
    suite = NULL;
  }
}

/**
 * Note that the problem_index depends on the number of instances a suite is defined with.
 *
 * @param suite The given suite.
 * @param problem_index The index of the problem to be returned.
 *
 * @return The problem of the suite defined by problem_index (NULL if this problem has been filtered out
 * from the suite).
 */
coco_problem_t *coco_suite_get_problem(coco_suite_t *suite, const size_t problem_index) {

  size_t function_idx = 0, instance_idx = 0, dimension_idx = 0;
  coco_suite_decode_problem_index(suite, problem_index, &function_idx, &dimension_idx, &instance_idx);

  return coco_suite_get_problem_from_indices(suite, function_idx, dimension_idx, instance_idx);
}

/**
 * The number of problems in the suite is computed as a product of the number of instances, number of
 * functions and number of dimensions and therefore doesn't account for any filtering done through the
 * suite_options parameter of the coco_suite function.
 *
 * @param suite The given suite.
 *
 * @return The number of problems in the suite.
 */
size_t coco_suite_get_number_of_problems(const coco_suite_t *suite) {
  return (suite->number_of_instances * suite->number_of_functions * suite->number_of_dimensions);
}


/**
 * @brief Returns the instances read from either a "year: YEAR" or "instances: NUMBERS" string.
 *
 * If both "year" and "instances" are given, the second is ignored (and a warning is raised). See the
 * coco_suite function for more information about the required format.
 */
static size_t *coco_suite_get_instance_indices(const coco_suite_t *suite, const char *suite_instance) {

  int year = -1;
  char *instances = NULL;
  const char *year_string = NULL;
  long year_found, instances_found;
  int parse_year = 1, parse_instances = 1;
  size_t *result = NULL;

  if (suite_instance == NULL)
    return NULL;

  year_found = coco_strfind(suite_instance, "year");
  instances_found = coco_strfind(suite_instance, "instances");

  if ((year_found < 0) && (instances_found < 0))
    return NULL;

  if ((year_found > 0) && (instances_found > 0)) {
    if (year_found < instances_found) {
      parse_instances = 0;
      coco_warning("coco_suite_get_instance_indices(): 'instances' suite option ignored because it follows 'year'");
    }
    else {
      parse_year = 0;
      coco_warning("coco_suite_get_instance_indices(): 'year' suite option ignored because it follows 'instances'");
    }
  }

  if ((year_found >= 0) && (parse_year == 1)) {
    if (coco_options_read_int(suite_instance, "year", &(year)) != 0) {
      year_string = coco_suite_get_instances_by_year(suite, year);
      result = coco_string_parse_ranges(year_string, 1, 0, "instances", COCO_MAX_INSTANCES);
    } else {
      coco_warning("coco_suite_get_instance_indices(): problems parsing the 'year' suite_instance option, ignored");
    }
  }

  instances = coco_allocate_string(COCO_MAX_INSTANCES);
  if ((instances_found >= 0) && (parse_instances == 1)) {
    if (coco_options_read_values(suite_instance, "instances", instances) > 0) {
      result = coco_string_parse_ranges(instances, 1, 0, "instances", COCO_MAX_INSTANCES);
    } else {
      coco_warning("coco_suite_get_instance_indices(): problems parsing the 'instance' suite_instance option, ignored");
    }
  }
  coco_free_memory(instances);

  return result;
}

/**
 * @brief Iterates through the items from the current_item_id position on in search for the next positive
 * item.
 *
 * If such an item is found, current_item_id points to this item and the method returns 1. If such an
 * item cannot be found, current_item_id points to the first positive item and the method returns 0.
 */
static int coco_suite_is_next_item_found(const size_t *items, const size_t number_of_items, long *current_item_id) {

  if ((*current_item_id) != (long) number_of_items - 1)  {
    /* Not the last item, iterate through items */
    do {
      (*current_item_id)++;
    } while (((*current_item_id) < (long) number_of_items - 1) && (items[*current_item_id] == 0));

    assert((*current_item_id) < (long) number_of_items);
    if (items[*current_item_id] != 0) {
      /* Next item is found, return true */
      return 1;
    }
  }

  /* Next item cannot be found, move to the first good item and return false */
  *current_item_id = -1;
  do {
    (*current_item_id)++;
  } while ((*current_item_id < (long) number_of_items - 1) && (items[*current_item_id] == 0));
  if (items[*current_item_id] == 0)
    coco_error("coco_suite_is_next_item_found(): the chosen suite has no valid (positive) items");
  return 0;
}

/**
 * @brief Iterates through the instances of the given suite from the current_instance_idx position on in
 * search for the next positive instance.
 *
 * If such an instance is found, current_instance_idx points to this instance and the method returns 1. If
 * such an instance cannot be found, current_instance_idx points to the first positive instance and the
 * method returns 0.
 */
static int coco_suite_is_next_instance_found(coco_suite_t *suite) {

  return coco_suite_is_next_item_found(suite->instances, suite->number_of_instances,
      &suite->current_instance_idx);
}

/**
 * @brief Iterates through the functions of the given suite from the current_function_idx position on in
 * search for the next positive function.
 *
 * If such a function is found, current_function_idx points to this function and the method returns 1. If
 * such a function cannot be found, current_function_idx points to the first positive function,
 * current_instance_idx points to the first positive instance and the method returns 0.
 */
static int coco_suite_is_next_function_found(coco_suite_t *suite) {

  int result = coco_suite_is_next_item_found(suite->functions, suite->number_of_functions,
      &suite->current_function_idx);
  if (!result) {
    /* Reset the instances */
    suite->current_instance_idx = -1;
    coco_suite_is_next_instance_found(suite);
  }
  return result;
}

/**
 * @brief Iterates through the dimensions of the given suite from the current_dimension_idx position on in
 * search for the next positive dimension.
 *
 * If such a dimension is found, current_dimension_idx points to this dimension and the method returns 1. If
 * such a dimension cannot be found, current_dimension_idx points to the first positive dimension and the
 * method returns 0.
 */
static int coco_suite_is_next_dimension_found(coco_suite_t *suite) {

  return coco_suite_is_next_item_found(suite->dimensions, suite->number_of_dimensions,
      &suite->current_dimension_idx);
}

/**
 * Currently, seven suites are supported:
 * - "bbob" contains 24 <a href="http://coco.lri.fr/downloads/download15.03/bbobdocfunctions.pdf">
 * single-objective functions</a> in 6 dimensions (2, 3, 5, 10, 20, 40)
 * - "bbob-biobj" contains 55 <a href="http://numbbo.github.io/coco-doc/bbob-biobj/functions">bi-objective
 * functions</a> in 6 dimensions (2, 3, 5, 10, 20, 40)
 * - "bbob-biobj-ext" as an extension of "bbob-biobj" contains 92
 * <a href="http://numbbo.github.io/coco-doc/bbob-biobj/functions">bi-objective functions</a> in 6 dimensions 
 * (2, 3, 5, 10, 20, 40)
 * - "bbob-largescale" contains 24 <a href="http://coco.lri.fr/downloads/download15.03/bbobdocfunctions.pdf">
 * single-objective functions</a> in 6 large dimensions (40, 80, 160, 320, 640, 1280)
 * - "bbob-constrained" contains 48 linearly-constrained problems, which are combinations of 8 single 
 * objective functions with 6 different numbers of linear constraints (1, 2, 10, dimension/2, dimension-1, 
 * dimension+1), in 6 dimensions (2, 3, 5, 10, 20, 40).
 * - "bbob-mixint" contains mixed-integer single-objective functions in 6 dimensions (2, 3, 5, 10, 20, 40)
 * - "bbob-biobj-mixint" contains 92 mixed-integer bi-objective functions in 6 dimensions (2, 3, 5, 10, 20, 40)
 * - "toy" contains 6 <a href="http://coco.lri.fr/downloads/download15.03/bbobdocfunctions.pdf">
 * single-objective functions</a> in 5 dimensions (2, 3, 5, 10, 20)
 *
 * Only the suite_name parameter needs to be non-empty. The suite_instance and suite_options can be "" or
 * NULL. In this case, default values are taken (default instances of a suite are those used in the last year
 * and the suite is not filtered by default).
 *
 * @param suite_name A string containing the name of the suite. Currently supported suite names are "bbob",
 * "bbob-biobj", "bbob-biobj-ext", "bbob-largescale", "bbob-constrained", and "toy".
 * @param suite_instance A string used for defining the suite instances. Two ways are supported:
 * - "year: YEAR", where YEAR is the year of the BBOB workshop, includes the instances (to be) used in that
 * year's workshop;
 * - "instances: VALUES", where VALUES are instance numbers from 1 on written as a comma-separated list or a
 * range m-n.
 * @param suite_options A string of pairs "key: value" used to filter the suite (especially useful for
 * parallelizing the experiments). Supported options:
 * - "dimensions: LIST", where LIST is the list of dimensions to keep in the suite (range-style syntax is
 * not allowed here),
 * - "dimension_indices: VALUES", where VALUES is a list or a range of dimension indices (starting from 1) to keep
 * in the suite, and
 * - "function_indices: VALUES", where VALUES is a list or a range of function indices (starting from 1) to keep
 * in the suite, and
 * - "instance_indices: VALUES", where VALUES is a list or a range of instance indices (starting from 1) to keep
 * in the suite.
 *
 * @return The constructed suite object.
 */
coco_suite_t *coco_suite(const char *suite_name, const char *suite_instance, const char *suite_options) {

  coco_suite_t *suite;
  size_t *instances;
  char *option_string = NULL;
  char *ptr;
  size_t *indices = NULL;
  size_t *dimensions = NULL;
  long dim_found, dim_idx_found;
  int parce_dim = 1, parce_dim_idx = 1;

  coco_option_keys_t *known_option_keys, *given_option_keys, *redundant_option_keys;

  /* Sets the valid keys for suite options and suite instance */
  const char *known_keys_o[] = { "dimensions", "dimension_indices", "function_indices", "instance_indices",
                                 "host_name", "port", "precision_x" };
  const char *known_keys_i[] = { "year", "instances" };

  /* Initialize the suite */
  suite = coco_suite_intialize(suite_name, suite_options);

  /* Set the instance */
  if ((!suite_instance) || (strlen(suite_instance) == 0))
    instances = coco_suite_get_instance_indices(suite, suite->default_instances);
  else {
    instances = coco_suite_get_instance_indices(suite, suite_instance);

    if (!instances) {
      /* Something wrong in the suite_instance string, use default instead */
      instances = coco_suite_get_instance_indices(suite, suite->default_instances);
    }

    /* Check for redundant option keys for suite instance */
    known_option_keys = coco_option_keys_allocate(sizeof(known_keys_i) / sizeof(char *), known_keys_i);
    given_option_keys = coco_option_keys(suite_instance);

    if (given_option_keys) {
      redundant_option_keys = coco_option_keys_get_redundant(known_option_keys, given_option_keys);

      if ((redundant_option_keys != NULL) && (redundant_option_keys->count > 0)) {
        /* Warn the user that some of given options are being ignored and output the valid options */
        char *output_redundant = coco_option_keys_get_output_string(redundant_option_keys,
            "coco_suite(): Some keys in suite instance were ignored:\n");
        char *output_valid = coco_option_keys_get_output_string(known_option_keys,
            "Valid keys for suite instance are:\n");
        coco_warning("%s%s", output_redundant, output_valid);
        coco_free_memory(output_redundant);
        coco_free_memory(output_valid);
      }

      coco_option_keys_free(given_option_keys);
      coco_option_keys_free(redundant_option_keys);
    }
    coco_option_keys_free(known_option_keys);
  }
  coco_suite_set_instance(suite, instances);
  coco_free_memory(instances);

  /* Apply filter if any given by the suite_options */
  if ((suite_options) && (strlen(suite_options) > 0)) {
    option_string = coco_allocate_string(COCO_PATH_MAX + 1);
    if (coco_options_read_values(suite_options, "function_indices", option_string) > 0) {
      indices = coco_string_parse_ranges(option_string, 1, suite->number_of_functions, "function_indices", COCO_MAX_INSTANCES);
      if (indices != NULL) {
        coco_suite_filter_indices(suite->functions, suite->number_of_functions, indices, "function_indices");
        coco_free_memory(indices);
      }
    }
    coco_free_memory(option_string);

    option_string = coco_allocate_string(COCO_PATH_MAX + 1);
    if (coco_options_read_values(suite_options, "instance_indices", option_string) > 0) {
      indices = coco_string_parse_ranges(option_string, 1, suite->number_of_instances, "instance_indices", COCO_MAX_INSTANCES);
      if (indices != NULL) {
        coco_suite_filter_indices(suite->instances, suite->number_of_instances, indices, "instance_indices");
        coco_free_memory(indices);
      }
    }
    coco_free_memory(option_string);

    dim_found = coco_strfind(suite_options, "dimensions");
    dim_idx_found = coco_strfind(suite_options, "dimension_indices");

    if ((dim_found > 0) && (dim_idx_found > 0)) {
      if (dim_found < dim_idx_found) {
        parce_dim_idx = 0;
        coco_warning("coco_suite(): 'dimension_indices' suite option ignored because it follows 'dimensions'");
      }
      else {
        parce_dim = 0;
        coco_warning("coco_suite(): 'dimensions' suite option ignored because it follows 'dimension_indices'");
      }
    }

    option_string = coco_allocate_string(COCO_PATH_MAX + 1);
    if ((dim_idx_found >= 0) && (parce_dim_idx == 1)
        && (coco_options_read_values(suite_options, "dimension_indices", option_string) > 0)) {
      indices = coco_string_parse_ranges(option_string, 1, suite->number_of_dimensions, "dimension_indices",
          COCO_MAX_INSTANCES);
      if (indices != NULL) {
        coco_suite_filter_indices(suite->dimensions, suite->number_of_dimensions, indices, "dimension_indices");
        coco_free_memory(indices);
      }
    }
    coco_free_memory(option_string);

    option_string = coco_allocate_string(COCO_PATH_MAX + 1);
    if ((dim_found >= 0) && (parce_dim == 1)
        && (coco_options_read_values(suite_options, "dimensions", option_string) > 0)) {
      ptr = option_string;
      /* Check for disallowed characters */
      while (*ptr != '\0') {
        if ((*ptr != ',') && !isdigit((unsigned char )*ptr)) {
          coco_warning("coco_suite(): 'dimensions' suite option ignored because of disallowed characters");
          return NULL;
        } else
          ptr++;
      }
      dimensions = coco_string_parse_ranges(option_string, suite->dimensions[0],
          suite->dimensions[suite->number_of_dimensions - 1], "dimensions", COCO_MAX_INSTANCES);
      if (dimensions != NULL) {
        coco_suite_filter_dimensions(suite, dimensions);
        coco_free_memory(dimensions);
      }
    }
    coco_free_memory(option_string);

    /* Check for redundant option keys for suite options */
    known_option_keys = coco_option_keys_allocate(sizeof(known_keys_o) / sizeof(char *), known_keys_o);
    given_option_keys = coco_option_keys(suite_options);

    if (given_option_keys) {
      redundant_option_keys = coco_option_keys_get_redundant(known_option_keys, given_option_keys);

      if ((redundant_option_keys != NULL) && (redundant_option_keys->count > 0)) {
        /* Warn the user that some of given options are being ignored and output the valid options */
        char *output_redundant = coco_option_keys_get_output_string(redundant_option_keys,
            "coco_suite(): Some keys in suite options were ignored:\n");
        char *output_valid = coco_option_keys_get_output_string(known_option_keys,
            "Valid keys for suite options are:\n");
        coco_warning("%s%s", output_redundant, output_valid);
        coco_free_memory(output_redundant);
        coco_free_memory(output_valid);
      }

      coco_option_keys_free(given_option_keys);
      coco_option_keys_free(redundant_option_keys);
    }
    coco_option_keys_free(known_option_keys);

  }

  /* Check that there are enough dimensions, functions and instances left */
  if ((suite->number_of_dimensions < 1)
      || (suite->number_of_functions < 1)
      || (suite->number_of_instances < 1)) {
    coco_error("coco_suite(): the suite does not contain at least one dimension, function and instance");
    return NULL;
  }

  /* Set the starting values of the current indices in such a way, that when the instance_idx is incremented,
   * this results in a valid problem */
  coco_suite_is_next_function_found(suite);
  coco_suite_is_next_dimension_found(suite);

  return suite;
}

/**
 * Iterates through the suite first by instances, then by functions and finally by dimensions.
 * The instances/functions/dimensions that have been filtered out using the suite_options of the coco_suite
 * function are skipped. Outputs some information regarding the current place in the iteration. The returned
 * problem is wrapped with the observer. If the observer is NULL, the returned problem is unobserved.
 *
 * @param suite The given suite.
 * @param observer The observer used to wrap the problem. If NULL, the problem is returned unobserved.
 *
 * @returns The next problem of the suite or NULL if there is no next problem left.
 */
coco_problem_t *coco_suite_get_next_problem(coco_suite_t *suite, coco_observer_t *observer) {
  
  size_t function_idx;
  size_t dimension_idx;
  size_t instance_idx;
  coco_problem_t *problem;

  long previous_function_idx;
  long previous_dimension_idx;
  long previous_instance_idx;

  assert(suite != NULL);

  previous_function_idx = suite->current_function_idx;
  previous_dimension_idx = suite->current_dimension_idx;
  previous_instance_idx = suite->current_instance_idx;

  /* Iterate through the suite by instances, then functions and lastly dimensions in search for the next
   * problem. Note that these functions set the values of suite fields current_instance_idx,
   * current_function_idx and current_dimension_idx. */
  if (!coco_suite_is_next_instance_found(suite)
      && !coco_suite_is_next_function_found(suite)
      && !coco_suite_is_next_dimension_found(suite)) {
    coco_info_partial("done\n");
    return NULL;
  }
 
  if (suite->current_problem) {
    coco_problem_free(suite->current_problem);
  }

  assert(suite->current_function_idx >= 0);
  assert(suite->current_dimension_idx >= 0);
  assert(suite->current_instance_idx >= 0);

  function_idx = (size_t) suite->current_function_idx;
  dimension_idx = (size_t) suite->current_dimension_idx;
  instance_idx = (size_t) suite->current_instance_idx;

  problem = coco_suite_get_problem_from_indices(suite, function_idx, dimension_idx, instance_idx);
  if (observer != NULL)
    problem = coco_problem_add_observer(problem, observer);
  suite->current_problem = problem;

  /* Output information regarding the current place in the iteration */
  if (coco_log_level >= COCO_INFO) {
    if (((long) dimension_idx != previous_dimension_idx) || (previous_instance_idx < 0)) {
      /* A new dimension started */
      char *time_string = coco_current_time_get_string();
      if (dimension_idx > 0)
        coco_info_partial("done\n");
      else
        coco_info_partial("\n");
      coco_info_partial("COCO INFO: %s, d=%lu, running: f%02lu", time_string,
          (unsigned long) suite->dimensions[dimension_idx], (unsigned long) suite->functions[function_idx]);
      coco_free_memory(time_string);
    }
    else if ((long) function_idx != previous_function_idx) {
      /* A new function started */
      coco_info_partial("f%02lu", (unsigned long) suite->functions[function_idx]);
    }
    /* One dot for each instance */
    coco_info_partial(".", suite->instances[instance_idx]);
  }

  return problem;
}

/* See coco.h for more information on encoding and decoding problem index */

/**
 * @param suite The suite.
 * @param function_idx Index of the function (starting with 0).
 * @param dimension_idx Index of the dimension (starting with 0).
 * @param instance_idx Index of the instance (starting with 0).
 *
 * @return The problem index in the suite computed from function_idx, dimension_idx and instance_idx.
 */
size_t coco_suite_encode_problem_index(const coco_suite_t *suite,
                                       const size_t function_idx,
                                       const size_t dimension_idx,
                                       const size_t instance_idx) {

  return instance_idx + (function_idx * suite->number_of_instances) +
      (dimension_idx * suite->number_of_instances * suite->number_of_functions);

}

/**
 * @param suite The suite.
 * @param problem_index Index of the problem in the suite (starting with 0).
 * @param function_idx Pointer to the index of the function, which is set by this function.
 * @param dimension_idx Pointer to the index of the dimension, which is set by this function.
 * @param instance_idx Pointer to the index of the instance, which is set by this function.
 */
void coco_suite_decode_problem_index(const coco_suite_t *suite,
                                     const size_t problem_index,
                                     size_t *function_idx,
                                     size_t *dimension_idx,
                                     size_t *instance_idx) {

  if (problem_index > (suite->number_of_instances * suite->number_of_functions * suite->number_of_dimensions) - 1) {
    coco_warning("coco_suite_decode_problem_index(): problem_index too large");
    function_idx = 0;
    instance_idx = 0;
    dimension_idx = 0;
    return;
  }

  *instance_idx = problem_index % suite->number_of_instances;
  *function_idx = (problem_index / suite->number_of_instances) % suite->number_of_functions;
  *dimension_idx = problem_index / (suite->number_of_instances * suite->number_of_functions);

}
