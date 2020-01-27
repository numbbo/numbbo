/**
 * @file transform_vars_permutation_helpers.c
 * @brief implements functions needed by transform_vars_permutation.c
 */

#include <stdio.h>
#include <assert.h>
#include "coco.h"

#include "coco_random.c"
#include "suite_bbob_legacy_code.c" /*tmp*/

#include <time.h> /*tmp*/

/* TODO: Document this file in doxygen style! */

static double *perm_random_data;/* global variable used to generate the random permutations */

/**
 * @brief Comparison function used for sorting. In our case, it serves as a random permutation generator
 */
static int f_compare_doubles_for_random_permutation(const void *a, const void *b) {
  double temp = perm_random_data[*(const size_t *) a] - perm_random_data[*(const size_t *) b];
  if (temp > 0)
    return 1;
  else if (temp < 0)
    return -1;
  else
    return 0;
}

/**
 * @brief generates a random, uniformly sampled, permutation and puts it in P
 * Wassim: move to coco_utilities?
 */
static void coco_compute_random_permutation(size_t *P, long seed, size_t n) {
  size_t i;

  perm_random_data = coco_allocate_vector(n);
  bbob2009_gauss(perm_random_data, n, seed);
  for (i = 0; i < n; i++){
    P[i] = i;
  }
  qsort(P, n, sizeof(size_t), f_compare_doubles_for_random_permutation);
  coco_free_memory(perm_random_data);
}


/**
 * @brief generates a permutation by sorting a sequence and puts it in P
 */
static void coco_compute_permutation_from_sequence(size_t *P, double *seq, size_t length) {
  size_t i;
    
  perm_random_data = coco_allocate_vector(length);
  for (i = 0; i < length; i++){
      P[i] = i;
      perm_random_data[i] = seq[i];
  }
  qsort(P, length, sizeof(size_t), f_compare_doubles_for_random_permutation);
  coco_free_memory(perm_random_data);
}


/**
 * @brief returns a uniformly distributed integer between lower_bound and upper_bound using seed
 * without using coco_random_new.
 * Move to coco_utilities?
 */
static long coco_random_unif_integer(long lower_bound, long upper_bound, long seed){
  long range, rand_int;
  double *tmp_uniform;
  tmp_uniform = coco_allocate_vector(1);
  bbob2009_unif(tmp_uniform, 1, seed);
  range = upper_bound - lower_bound + 1;
  rand_int = ((long)(tmp_uniform[0] * (double) range)) + lower_bound;
  coco_free_memory(tmp_uniform);
  return rand_int;
}


/**
 * @brief generates a random permutation resulting from nb_swaps truncated uniform swaps of range swap_range
 * missing parameters: dynamic_not_static pool, seems empirically irrelevant
 * for now so dynamic is implemented (simple since no need for tracking indices
 * if swap_range is 0, a random uniform permutation is generated
 */
static void coco_compute_truncated_uniform_swap_permutation(size_t *P, long seed, size_t n, size_t nb_swaps, size_t swap_range) {
  size_t i, idx_swap;
  size_t lower_bound, upper_bound, first_swap_var, second_swap_var, tmp;
  size_t *idx_order;

  if (n <= 40) {
    /* Do an identity permutation for dimensions <= 40 */
    for (i = 0; i < n; i++)
      P[i] = i;
    return;
  }

  perm_random_data = coco_allocate_vector(n);
  bbob2009_unif(perm_random_data, n, seed);

  idx_order = coco_allocate_vector_size_t(n);
  for (i = 0; i < n; i++) {
    P[i] = i;
    idx_order[i] = i;
  }

  if (swap_range > 0) {
    /*sort the random data in perm_random_data and arrange idx_order accordingly*/
    /*did not use coco_compute_random_permutation to only use the seed once*/
    qsort(idx_order, n, sizeof(size_t), f_compare_doubles_for_random_permutation);
    for (idx_swap = 0; idx_swap < nb_swaps; idx_swap++) {
      first_swap_var = idx_order[idx_swap];
      if (first_swap_var < swap_range) {
        lower_bound = 0;
      }
      else{
        lower_bound = first_swap_var - swap_range;
      }
      if (first_swap_var + swap_range > n - 1) {
        upper_bound = n - 1;
      }
      else{
        upper_bound = first_swap_var + swap_range;
      }

      second_swap_var = (size_t) coco_random_unif_integer((long) lower_bound,
                                                          (long) upper_bound - 1,
                                                          seed + (long) (1 + idx_swap) * 1000);
      if (second_swap_var >= first_swap_var) {
        second_swap_var += 1;
      }
      /* swap*/
      tmp = P[first_swap_var];
      P[first_swap_var] = P[second_swap_var];
      P[second_swap_var] = tmp;
    }
  } else {
    /* generate random permutation instead */
    coco_compute_random_permutation(P, seed, n);
  }
  coco_free_memory(idx_order);
  coco_free_memory(perm_random_data);
}



/**
 * @brief duplicates a size_t vector
 */
static size_t *coco_duplicate_size_t_vector(const size_t *src, const size_t number_of_elements) {
  size_t i;
  size_t *dst;

  assert(src != NULL);
  assert(number_of_elements > 0);

  dst = coco_allocate_vector_size_t(number_of_elements);
  for (i = 0; i < number_of_elements; ++i) {
    dst[i] = src[i];
  }
  return dst;
}



/**
 * @brief return the swap_range corresponding to the problem in the given suite
 */
static size_t coco_get_swap_range(size_t dimension, const char *suite_name){
  if (strcmp(suite_name, "bbob-largescale") == 0) {
    return dimension / 3;
  } else {
    coco_error("coco_get_swap_range(): unknown problem suite");
    return (size_t) NULL;
  }
}


/**
 * @brief return the number of swaps corresponding to the problem in the given suite
 */
size_t coco_get_nb_swaps(size_t dimension, const char *suite_name){
  if (strcmp(suite_name, "bbob-largescale") == 0) {
    return dimension;
  } else {
    coco_error("coco_get_nb_swaps(): unknown problem suite");
    return (size_t) NULL;
  }
}
