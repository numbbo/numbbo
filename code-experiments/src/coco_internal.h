/**
 * @file coco_internal.h
 * @brief Definitions of internal COCO structures and typedefs.
 *
 * These are used throughout the COCO code base but should not be used by any external code.
 */

#ifndef __COCO_INTERNAL__
#define __COCO_INTERNAL__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************/
/**
 * @brief The data free function type.
 *
 * This is a template for functions that free the contents of data (used to free the contents of data
 * fields in coco_problem, coco_suite and coco_observer).
 */
typedef void (*coco_data_free_function_t)(void *data);

/**
 * @brief The problem free function type.
 *
 * This is a template for functions that free the problem structure.
 */
typedef void (*coco_problem_free_function_t)(coco_problem_t *problem);

/**
 * @brief The evaluate function type.
 *
 * This is a template for functions that perform an evaluation of the problem (to evaluate the problem
 * function, the problems constraints etc.).
 */
typedef void (*coco_evaluate_function_t)(coco_problem_t *problem, const double *x, double *y);

/**
 * @brief The recommend solutions function type.
 *
 * This is a template for functions that log a recommended solution.
 */
typedef void (*coco_recommend_function_t)(coco_problem_t *problem, const double *x);

/**
 * @brief The allocate logger function type.
 *
 * This is a template for functions that allocate a logger (wrap a logger around the given problem and return
 * the wrapped problem).
 */
typedef coco_problem_t *(*coco_logger_allocate_function_t)(coco_observer_t *observer,
                                                           coco_problem_t *problem);
/**
 * @brief The free logger function type.
 *
 * This is a template for functions that free a logger.
 */
typedef void (*coco_logger_free_function_t)(void *logger);

/**
 * @brief The get problem function type.
 *
 * This is a template for functions that return a problem based on function, dimension and instance.
 */
typedef coco_problem_t *(*coco_get_problem_function_t)(const size_t function,
                                                       const size_t dimension,
                                                       const size_t instance);

/**
 * @brief The transformed COCO problem data type.
 *
 * This is a type of a generic structure for a transformed ("outer") coco_problem. It makes possible the
 * wrapping of problems as layers of an onion. Initialized in the coco_problem_transformed_allocate function,
 * it makes the current ("outer") transformed problem a "derived problem class", which inherits from the
 * "inner" problem, the "base class".
 *
 * From the perspective of the inner problem:
 * - data holds the meta-information to administer the inheritance
 * - data->data holds the additional fields of the derived class (the outer problem)
 * - data->inner_problem points to the inner problem (now we have a linked list)
 */
typedef struct {
  coco_problem_t *inner_problem;                  /**< @brief Pointer to the inner problem */
  void *data;                                     /**< @brief Pointer to data, which enables further
                                                  wrapping of the problem */
  coco_data_free_function_t data_free_function;   /**< @brief Function to free the contents of data */
} coco_problem_transformed_data_t;

/**
 * @brief The stacked COCO problem data type.
 *
 * This is a type of a structure used when stacking two problems (especially useful for constructing
 * multi-objective problems).
 */
typedef struct {
  coco_problem_t *problem1; /**< @brief Pointer to the first problem (objective) */
  coco_problem_t *problem2; /**< @brief Pointer to the second problem (objective) */
} coco_problem_stacked_data_t;

/**
 * @brief The option keys data type.
 *
 * This is a type of a structure used to contain a set of known option keys (used by suites and observers).
 */
typedef struct {
  size_t count;  /**< @brief Number of option keys */
  char **keys;   /**< @brief Pointer to option keys */
} coco_option_keys_t;


/***********************************************************************************************************/

/**
 * @brief The COCO problem structure.
 *
 * This is one of the main structures in COCO. It contains information about a problem to be optimized. The
 * problems can be wrapped around each other (similar to the onion layers) by means of the data field and
 * the coco_problem_transformed_data_t structure creating some kind of "object inheritance". Even the logger
 * is considered as just another coco_problem instance wrapped around the original problem.
 */
struct coco_problem_s {

  coco_evaluate_function_t evaluate_function;         /**< @brief  The function for evaluating the problem. */
  coco_evaluate_function_t evaluate_constraint;       /**< @brief  The function for evaluating the constraints. */
  coco_evaluate_function_t evaluate_gradient;         /**< @brief  The function for evaluating the constraints. */
  coco_recommend_function_t recommend_solution;       /**< @brief  The function for recommending a solution. */
  coco_problem_free_function_t problem_free_function; /**< @brief  The function for freeing this problem. */

  size_t number_of_variables;          /**< @brief Number of variables expected by the function, i.e.
                                       problem dimension */
  size_t number_of_objectives;         /**< @brief Number of objectives. */
  size_t number_of_constraints;        /**< @brief Number of constraints. */

  double *smallest_values_of_interest; /**< @brief The lower bounds of the ROI in the decision space. */
  double *largest_values_of_interest;  /**< @brief The upper bounds of the ROI in the decision space. */
  size_t number_of_integer_variables;  /**< @brief Number of integer variables (if > 0, all integer variables come
                                       before any continuous ones). */

  double *initial_solution;            /**< @brief Initial feasible solution. */
  double *best_value;                  /**< @brief Optimal (smallest) function value */
  double *nadir_value;                 /**< @brief The nadir point (defined when number_of_objectives > 1) */
  double *best_parameter;              /**< @brief Optimal decision vector (defined only when unique) */

  char *problem_name;                  /**< @brief Problem name. */
  char *problem_id;                    /**< @brief Problem ID (unique in the containing suite) */
  char *problem_type;                  /**< @brief Problem type */

  size_t evaluations;                  /**< @brief Number of objective function evaluations performed on the problem. */
  size_t evaluations_constraints;      /**< @brief Number of constraint function evaluations performed on the problem. */

  /* Convenience fields for output generation */
  /* If at some point in time these arrays are changed to pointers, checks need to be added in the code to make sure
   * they are not NULL.*/

  double final_target_delta[1];        /**< @brief Final target delta. */
  double best_observed_fvalue[1];      /**< @brief The best observed value so far. */
  size_t best_observed_evaluation[1];  /**< @brief The evaluation when the best value so far was achieved. */

  /* Fields depending on the containing benchmark suite */

  coco_suite_t *suite;                 /**< @brief Pointer to the containing suite (NULL if not given) */
  size_t suite_dep_index;              /**< @brief Suite-depending problem index (starting from 0) */
  size_t suite_dep_function;           /**< @brief Suite-depending function */
  size_t suite_dep_instance;           /**< @brief Suite-depending instance */

  void *data;                          /**< @brief Pointer to a data instance @see coco_problem_transformed_data_t */
  
  void *versatile_data;                /* Wassim: *< @brief pointer to eventual additional data that need to be accessed all along the transformations*/
};

/**
 * @brief The COCO observer structure.
 *
 * An observer observes the whole benchmark process. It is independent of suites and problems. Each time a
 * new problem of the suite is being observed, the observer initializes a new logger (wraps the observed
 * problem with the corresponding logger).
 */
struct coco_observer_s {

  int is_active;             /**< @brief Whether the observer is active (the logger will log some output). */
  char *observer_name;       /**< @brief Name of the observer for identification purposes. */
  char *result_folder;       /**< @brief Name of the result folder. */
  char *algorithm_name;      /**< @brief Name of the algorithm to be used in logger output. */
  char *algorithm_info;      /**< @brief Additional information on the algorithm to be used in logger output. */
  size_t number_target_triggers;
                             /**< @brief The number of targets between each 10**i and 10**(i+1). */
  double target_precision;   /**< @brief The minimal precision used for targets. */
  size_t number_evaluation_triggers;
                             /**< @brief The number of triggers between each 10**i and 10**(i+1) evaluation number. */
  char *base_evaluation_triggers;
                             /**< @brief The "base evaluations" used to evaluations that trigger logging. */
  int precision_x;           /**< @brief Output precision for decision variables. */
  int precision_f;           /**< @brief Output precision for function values. */
  int precision_g;           /**< @brief Output precision for constraint values. */
  int log_discrete_as_int;   /**< @brief Whether to output discrete variables in int or double format. */
  void *data;                /**< @brief Void pointer that can be used to point to data specific to an observer. */

  coco_data_free_function_t data_free_function;             /**< @brief  The function for freeing this observer. */
  coco_logger_allocate_function_t logger_allocate_function; /**< @brief  The function for allocating the logger. */
  coco_logger_free_function_t logger_free_function;         /**< @brief  The function for freeing the logger. */
};

/**
 * @brief The COCO suite structure.
 *
 * A suite is a collection of problems constructed by a Cartesian product of the suite's optimization
 * functions, dimensions and instances. The functions and dimensions are fixed for a suite with some name,
 * while the instances are defined dynamically. The suite can be filtered - only the chosen functions,
 * dimensions and instances will be taken into account when iterating through the suite.
 */
struct coco_suite_s {

  char *suite_name;                /**< @brief Name of the suite. */

  size_t number_of_dimensions;     /**< @brief Number of dimensions contained in the suite. */
  size_t *dimensions;              /**< @brief The dimensions contained in the suite. */

  size_t number_of_functions;      /**< @brief Number of functions contained in the suite. */
  size_t *functions;               /**< @brief The functions contained in the suite. */

  size_t number_of_instances;      /**< @brief Number of instances contained in the suite. */
  char *default_instances;         /**< @brief The instances contained in the suite by default. */
  size_t *instances;               /**< @brief The instances contained in the suite. */

  coco_problem_t *current_problem; /**< @brief Pointer to the currently tackled problem. */
  long current_dimension_idx;      /**< @brief The dimension index of the currently tackled problem. */
  long current_function_idx;       /**< @brief The function index of the currently tackled problem. */
  long current_instance_idx;       /**< @brief The instance index of the currently tackled problem. */

  void *data;                      /**< @brief Void pointer that can be used to point to data specific to a suite. */

  coco_data_free_function_t data_free_function; /**< @brief The function for freeing this suite. */

};

static void bbob_evaluate_gradient(coco_problem_t *problem, const double *x, double *y);

void bbob_problem_best_parameter_print(const coco_problem_t *problem);
void bbob_biobj_problem_best_parameter_print(const coco_problem_t *problem);

#ifdef __cplusplus
}
#endif
#endif

