/**
 * @file  suite_cons_bbob_problems.c
 * @brief Implementation of the problems in the constrained BBOB suite.
 * 
 * This suite contains 48 constrained functions in continuous domain 
 * which are derived from combining 8 single-objective functions of the
 * noiseless bbob test suite with randomly-generated 
 * linear constraints perturbed by nonlinear transformations.
 * Each one of the 8 functions is combined with 6 different numbers of 
 * constraints: 1, 2, 6, 6+n/2, 6+n and 6+3n.
 * 
 * We consider constrained optimization problems of the form
 * 
 *     min f(x) s.t. g_i(x) <= 0, i=1,...,l.
 * 
 * The constrained functions are built in a way such that the 
 * KKT conditions are satisied at the origin, which is the initial
 * optimal solution. We then translate the constrained function
 * by a random vector in order to move the optimal solution away
 * from the origin. 
 * 
 * ***************************************************
 * General idea for building the constrained functions
 * ***************************************************
 * 
 * 1. Choose a bbob function f whose raw version is pseudoconvex
 *    to be the objective function. (see note below)
 * 2. Remove possible nonlinear transformations from f. (see note below)
 * 3. In order to make sure that the feasible set is not empty,
 *    we choose a direction p that should be feasible.
 *    Define p as the gradient of f at the origin.
 * 4. Define the first constraint g_1(x)=a_1'*x by setting its gradient
 *    to a_1 = -p. By definition, g_1(p) < 0. Thus p is feasible
 *    for g_1.
 * 5. Generate the other constraints randomly while making sure that 
 *    p remains feasible for each one.
 * 6. Apply to the whole constrained function the nonlinear transformations 
 *    that were removed from the objective function in Step 2. (see note below)
 * 7. Choose a random point xopt and move the optimum away from the 
 *    origin to xopt by translating the constrained function by -xopt.
 * 
 * @note The pseudoconvexity in Step 1 guarantees that the KKT conditions 
 *       are sufficient for optimality.
 * 
 * @note The removal of possible nonlinear transformations in Step 2
 *       and posterior application in Step 6 are necessary to make
 *       sure that the KKT conditions are satisfied in the optimum
 *       - until then the origin. As explained in the documentation, 
 *       the application of the nonlinear transformations in Step 6 
 *       does not affect the location of the optimum.
 * 
 * @note a_1 is set to -p, i.e. the negative gradient of f at the origin, 
 *       in order to have the KKT conditions easily satisfied.
 *
 * @note Steps 1 and 2 are done within the 'allocate' function of the
 *       objective function, e.g. f_ellipsoid_cons_bbob_problem_allocate().
 * 
 * @note Steps 4 and 5 are done within c_linear_cons_bbob_problem_allocate().
 * 
 * @note The constrained Rastrigin function's construction differs a bit
 *       from the steps above. Since it is a multimodal function with
 *       well distributed local optima, we choose one of its local optima
 *       to be the global constrained optimum by adding constraints 
 *       that pass through that point.
 * 
 * @note The testbed provides the user an initial solution which is given
 *       by the feasible direction p scaled by some constant.
 * 
 * *************************************************
 * COCO data structure for the constrained functions
 * *************************************************
 * 
 * First, we create a coco_problem_t object for the objective function.
 * Then, coco_problem_t objects are created for each constraint and
 * stacked together into a single coco_problem_t object (see "c_linear.c"). 
 * Finally, the coco_problem_t object containing the constraints and 
 * the coco_problem_t object containing the objective function are 
 * stacked together to form the constrained function.
 * 
 */

#include <math.h>

#include "coco.h"
#include "coco_utilities.c"
#include "coco_random.c"
#include "c_linear.c"
#include "f_different_powers.c"
#include "f_discus.c"
#include "f_ellipsoid.c"
#include "f_linear_slope.c"
#include "f_rastrigin.c"
#include "f_sphere.c"

/**
 * @brief Calculates the objective function type based on the value
 *        of "function"
 */
static size_t obj_function_type(const size_t function) {
  
  size_t problems_per_obj_function_type = 6;
  return (size_t)ceil((double)function/(double)problems_per_obj_function_type);
  
}

/**
 * @brief Returns the number of linear constraints associated to the
 *        value of "function"
 */
static size_t nb_of_linear_constraints(const size_t function,
                                       const size_t dimension) {
  
  int problems_per_obj_function_type = 6;
  int p;
  
  /* Map "function" value into {1, ..., problems_per_obj_function_type} */
  p = (((int)function - 1) % problems_per_obj_function_type) + 1;
  
  if (p == 1) return 1;
  else if (p == 2) return 2;
  else if (p == 3) return 6;
  else if (p == 4) return 6 + dimension/2;
  else if (p == 5) return 6 + dimension;
  else return 6 + 3*dimension;
  
}

/**
 * @brief Scale feasible direction depending on xopt such that
 *        xopt + feasible_direction remains in [-5, 5].
 *
 *
 */
static void feasible_direction_set_length(double * feasible_direction,
                                          const double *xopt,
                                          size_t dimension,
                                          long rseed) {
  const long seed_offset = 412;  /* was sampled uniform in 0-999 */
  const double feas_shrink = 0.75;  /* scale randomly between 0.75 and 1.0 */
  const double feas_bound = 5.0;

  size_t i;
  double r[1], maxabs, maxrel;

  maxabs = maxrel = 0.0;
  for (i = 0; i < dimension; ++i) {
    maxabs = coco_double_max(maxabs, fabs(xopt[i]));
    maxrel = coco_double_max(maxrel, feasible_direction[i] / (feas_bound - xopt[i]));
    maxrel = coco_double_max(maxrel, feasible_direction[i] / (-feas_bound - xopt[i]));
  }
  if (maxabs > 4.01)
    coco_warning("feasible_direction_set_length: a component of fabs(xopt) was greater than 4.01");
  if (maxabs > 5.0)
    coco_error("feasible_direction_set_length: a component of fabs(xopt) was greater than 5.0");
  bbob2009_unif(r, 1, rseed + seed_offset);
  coco_vector_scale(feasible_direction, dimension,
                    feas_shrink + r[0] * (1 - feas_shrink),  /* nominator */
                    maxrel);  /* denominator */
}

/**
 * @brief Objective function: sphere
 *        Constraint(s): linear
 */
static coco_problem_t *f_sphere_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
                                                         		
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;
  
  char *problem_type_temp = NULL;
  double *all_zeros = NULL;  
  
  all_zeros = coco_allocate_vector(dimension);             
  
  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;    
  
  /* Create the objective function */
  problem = f_sphere_bbob_problem_allocate(function, dimension, 
      instance, rseed, problem_id_template, problem_name_template);
	 
  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);	 
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);
	 
  /* Create the constraints. Use the gradient of the objective
   * function at the origin to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function, 
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
	    
  problem_type_temp = coco_strdup(problem->problem_type);
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
	    
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value.
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;
     
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin.
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
	
  return problem;
 
}

/**
 * @brief Objective function: ellipsoid
 *        Constraint(s): linear
 */
static coco_problem_t *f_ellipsoid_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
																			
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;

  char *problem_type_temp = NULL;
  double *all_zeros = NULL;
  
  all_zeros = coco_allocate_vector(dimension);
 
  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;
     
  /* Create the objective function */
  problem = f_ellipsoid_cons_bbob_problem_allocate(function, dimension, 
      instance, rseed, problem_id_template, problem_name_template);

  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);

  /* Create the constraints. Use the gradient of the objective
   * function at the origin to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function, 
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
      
  problem_type_temp = coco_strdup(problem->problem_type);
  
  /* Build the final constrained function by stacking the objective
   * function and the constraints into one coco_problem_t type.
   */
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
  
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value.
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;

  problem = transform_vars_oscillate(problem);
     
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin.
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
  
  return problem;
 
}

/**
 * @brief Objective function: rotated ellipsoid
 *        Constraint(s): linear
 */
static coco_problem_t *f_ellipsoid_rotated_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
																			
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;
  
  char *problem_type_temp = NULL;
  double *all_zeros = NULL;
  
  all_zeros = coco_allocate_vector(dimension);
 
  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;
	 
  /* Create the objective function */
  problem = f_ellipsoid_rotated_cons_bbob_problem_allocate(function, dimension, 
      instance, rseed, problem_id_template, problem_name_template);
      
  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);
  
  /* Create the constraints. Use the gradient of the objective
   * function at the origin to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function, 
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
      
  problem_type_temp = coco_strdup(problem->problem_type);
  
  /* Build the final constrained function by stacking the objective
   * function and the constraints into one coco_problem_t type.
   */
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
     
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value.
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;
  
  problem = transform_vars_oscillate(problem);
     
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin .
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
  
  return problem;
 
}

/**
 * @brief Objective function: linear slope
 *        Constraint(s): linear
 */
static coco_problem_t *f_linear_slope_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
																			
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;
  
  char *problem_type_temp = NULL;
  double *all_zeros = NULL;
  
  all_zeros = coco_allocate_vector(dimension);
 
  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;
	 
  /* Create the objective function */
  problem = f_linear_slope_bbob_problem_allocate(function, dimension, 
      instance, rseed, problem_id_template, problem_name_template);
      
  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);
  
  /* Create the constraints. Use the gradient of the objective
   * function at the origin to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function, 
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
      
  problem_type_temp = coco_strdup(problem->problem_type);
  
  /* Build the final constrained function by stacking the objective
   * function and the constraints into one coco_problem_t type.
   */
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
  
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value.
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;
     
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin.
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
  
  return problem;
 
}

/**
 * @brief Objective function: discus
 *        Constraint(s): linear
 */
static coco_problem_t *f_discus_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
																			
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;
  
  char *problem_type_temp = NULL;
  double *all_zeros = NULL;
  
  all_zeros = coco_allocate_vector(dimension);
 
  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;

  /* Create the objective function */
  problem = f_discus_cons_bbob_problem_allocate(function, dimension, 
      instance, rseed, problem_id_template, problem_name_template);
      
  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);

  /* Create the constraints. Use the gradient of the objective
   * function at the origin to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function, 
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
      
  problem_type_temp = coco_strdup(problem->problem_type);
  
  /* Build the final constrained function by stacking the objective
   * function and the constraints into one coco_problem_t type.
   */
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
  
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;
     
  problem = transform_vars_oscillate(problem);
     
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin. 
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
  
  return problem;
 
}

/**
 * @brief Objective function: bent cigar
 *        Constraint(s): linear
 */
static coco_problem_t *f_bent_cigar_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
																			
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;
  
  char *problem_type_temp = NULL;
  double *all_zeros = NULL;
  
  all_zeros = coco_allocate_vector(dimension);
 
  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;
	 
  /* Create the objective function */
  problem = f_bent_cigar_cons_bbob_problem_allocate(function, dimension, 
      instance, rseed, problem_id_template, problem_name_template);
      
  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);

  /* Create the constraints. Use the gradient of the objective
   * function at the origin to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function, 
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
      
  problem_type_temp = coco_strdup(problem->problem_type);
  
  /* Build the final constrained function by stacking the objective
   * function and the constraints into one coco_problem_t type.
   */
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
  
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value.
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;
     
  problem = transform_vars_asymmetric(problem, 0.2);
     
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin. 
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
  
  return problem;
 
}

/**
 * @brief Objective function: different powers
 *        Constraint(s): linear
 */
static coco_problem_t *f_different_powers_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
																			
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;
  
  char *problem_type_temp = NULL;
  double *all_zeros = NULL;
  
  all_zeros = coco_allocate_vector(dimension);
 
  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;
	 
  /* Create the objective function */
  problem = f_different_powers_bbob_constrained_problem_allocate(function, dimension,
      instance, rseed, problem_id_template, problem_name_template);
      
  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);
  
  /* Create the constraints. Use the gradient of the objective
   * function at the origin to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function, 
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
      
  problem_type_temp = coco_strdup(problem->problem_type);
  
  /* Build the final constrained function by stacking the objective
   * function and the constraints into one coco_problem_t type.
   */
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
  
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value.
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;
     
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin.
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
  
  return problem;
 
}

/**
 * @brief Objective function: rastrigin
 *        Constraint(s): linear
 */
static coco_problem_t *f_rastrigin_c_linear_cons_bbob_problem_allocate(const size_t function,
                                                      const size_t dimension,
                                                      const size_t instance,
                                                      const size_t number_of_linear_constraints,
                                                      const long rseed,
                                                      double *feasible_direction,
                                                      const double *xopt,
                                                      const char *problem_id_template,
                                                      const char *problem_name_template) {
																			
  size_t i;
  coco_problem_t *problem = NULL;
  coco_problem_t *problem_c = NULL;

  double *all_zeros = NULL;
  char *problem_type_temp = NULL;

  all_zeros = coco_allocate_vector(dimension);

  for (i = 0; i < dimension; ++i)
    all_zeros[i] = 0.0;

  /* Create the objective function */
  problem = f_rastrigin_cons_bbob_problem_allocate(function, dimension, 
      instance, rseed, problem_id_template, problem_name_template);
  
  bbob_evaluate_gradient(problem, all_zeros, feasible_direction);
  feasible_direction_set_length(feasible_direction, xopt, dimension, rseed);
     
  /* Create the constraints. Use the feasible direction above
   * to build the first constraint. 
   */
  problem_c = c_linear_cons_bbob_problem_allocate(function,
      dimension, instance, number_of_linear_constraints,
      problem_id_template, problem_name_template, feasible_direction);
      
  problem_type_temp = coco_strdup(problem->problem_type);
  
  /* Build the final constrained function by stacking the objective
   * function and the constraints into one coco_problem_t type.
   */
  problem = coco_problem_stacked_allocate(problem, problem_c,
      problem_c->smallest_values_of_interest, 
      problem_c->largest_values_of_interest);
  
  /* Define problem->best_parameter as the origin and store its
   * objective function value into problem->best_value.
   */
  for (i = 0; i < dimension; ++i)
    problem->best_parameter[i] = 0.0;
  assert(problem->evaluate_function != NULL);
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  
  /* To be sure that everything is correct, reset the f-evaluations
   * and g-evaluations counters to zero.
   */
  problem->evaluations = 0;
  problem->evaluations_constraints = 0;
  
  problem = transform_vars_asymmetric(problem, 0.2);
  problem = transform_vars_oscillate(problem);
  
  /* Apply a translation to the whole problem so that the constrained 
   * minimum is no longer at the origin. 
   */
  problem = transform_vars_shift(problem, xopt, 0);
 
  /* Construct problem type */
  coco_problem_set_type(problem, "%s_%s", problem_type_temp, 
  problem_c->problem_type);
 
  coco_free_memory(problem_type_temp);
  coco_free_memory(all_zeros);
  
  return problem;
 
}
