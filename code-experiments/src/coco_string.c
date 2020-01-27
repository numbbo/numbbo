/**
 * @file coco_string.c
 * @brief Definitions of functions that manipulate strings.
 */

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "coco.h"

static size_t *coco_allocate_vector_size_t(const size_t number_of_elements);
static char *coco_allocate_string(const size_t number_of_elements);

/**
 * @brief Creates a duplicate copy of string and returns a pointer to it.
 *
 * The caller is responsible for freeing the allocated memory using coco_free_memory().
 */
static char *coco_strdup(const char *string) {
  size_t len;
  char *duplicate;
  if (string == NULL)
    return NULL;
  len = strlen(string);
  duplicate = (char *) coco_allocate_memory(len + 1);
  memcpy(duplicate, string, len + 1);
  return duplicate;
}

/**
 * @brief The length of the buffer used in the coco_vstrdupf function.
 *
 * @note This should be handled differently!
 */
#define COCO_VSTRDUPF_BUFLEN 4444

/**
 * @brief Formatted string duplication, with va_list arguments.
 */
static char *coco_vstrdupf(const char *str, va_list args) {
  static char buf[COCO_VSTRDUPF_BUFLEN];
  long written;
  /* apparently args can only be used once, therefore
   * len = vsnprintf(NULL, 0, str, args) to find out the
   * length does not work. Therefore we use a buffer
   * which limits the max length. Longer strings should
   * never appear anyway, so this is rather a non-issue. */

#if 0
  written = vsnprintf(buf, COCO_VSTRDUPF_BUFLEN - 2, str, args);
  if (written < 0)
  coco_error("coco_vstrdupf(): vsnprintf failed on '%s'", str);
#else /* less safe alternative, if vsnprintf is not available */
  assert(strlen(str) < COCO_VSTRDUPF_BUFLEN / 2 - 2);
  if (strlen(str) >= COCO_VSTRDUPF_BUFLEN / 2 - 2)
    coco_error("coco_vstrdupf(): string is too long");
  written = vsprintf(buf, str, args);
  if (written < 0)
    coco_error("coco_vstrdupf(): vsprintf failed on '%s'", str);
#endif
  if (written > COCO_VSTRDUPF_BUFLEN - 3)
    coco_error("coco_vstrdupf(): A suspiciously long string is tried to being duplicated '%s'", buf);
  return coco_strdup(buf);
}

#undef COCO_VSTRDUPF_BUFLEN

/**
 * Optional arguments are used like in sprintf.
 */
char *coco_strdupf(const char *str, ...) {
  va_list args;
  char *s;

  va_start(args, str);
  s = coco_vstrdupf(str, args);
  va_end(args);
  return s;
}

/**
 * @brief Returns a concatenate copy of string1 + string2.
 *
 * The caller is responsible for freeing the allocated memory using coco_free_memory().
 */
static char *coco_strconcat(const char *s1, const char *s2) {
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);
  char *s = (char *) coco_allocate_memory(len1 + len2 + 1);

  memcpy(s, s1, len1);
  memcpy(&s[len1], s2, len2 + 1);
  return s;
}

/**
 * @brief Returns the first index where seq occurs in base and -1 if it doesn't.
 *
 * @note If there is an equivalent standard C function, this can/should be removed.
 */
static long coco_strfind(const char *base, const char *seq) {
  const size_t strlen_seq = strlen(seq);
  const size_t last_first_idx = strlen(base) - strlen(seq);
  size_t i, j;

  if (strlen(base) < strlen(seq))
    return -1;

  for (i = 0; i <= last_first_idx; ++i) {
    if (base[i] == seq[0]) {
      for (j = 0; j < strlen_seq; ++j) {
        if (base[i + j] != seq[j])
          break;
      }
      if (j == strlen_seq) {
        if (i > 1e9)
          coco_error("coco_strfind(): strange values observed i=%lu, j=%lu, strlen(base)=%lu",
          		(unsigned long) i, (unsigned long) j, (unsigned long) strlen(base));
        return (long) i;
      }
    }
  }
  return -1;
}

/**
 * @brief Splits a string based on the given delimiter.
 *
 * Returns a pointer to the resulting substrings with NULL as the last one.
 * The caller is responsible for freeing the allocated memory using:
 *
 *  for (i = 0; *(result + i); i++)
 *    coco_free_memory(*(result + i));
 *  coco_free_memory(*(result + i));    <- This is needed!
 *  coco_free_memory(result);
 *
 */
static char **coco_string_split(const char *string, const char delimiter) {

  char **result;
  char *str_copy, *ptr, *token;
  char str_delimiter[2];
  size_t i;
  size_t count = 1;

  str_copy = coco_strdup(string);

  /* Counts the parts between delimiters */
  ptr = str_copy;
  while (*ptr != '\0') {
    if (*ptr == delimiter) {
      count++;
    }
    ptr++;
  }
  /* Makes room for an empty string that will be appended at the end */
  count++;

  result = (char **) coco_allocate_memory(count * sizeof(char *));

  /* Iterates through tokens
   * NOTE: strtok() ignores multiple delimiters, therefore the final number of detected substrings might be
   * lower than the count. This is OK. */
  i = 0;
  /* A char* delimiter needs to be used, otherwise strtok() can surprise */
  str_delimiter[0] = delimiter;
  str_delimiter[1] = '\0';
  token = strtok(str_copy, str_delimiter);
  while (token)
  {
      assert(i < count);
      *(result + i++) = coco_strdup(token);
      token = strtok(NULL, str_delimiter);
  }
  *(result + i) = NULL;

  coco_free_memory(str_copy);

  return result;
}

/**
 * @brief Creates and returns a string with removed characters between from and to.
 *
 * If you wish to remove characters from the beginning of the string, set from to "".
 * If you wish to remove characters until the end of the string, set to to "".
 *
 * The caller is responsible for freeing the allocated memory using coco_free_memory().
 */
static char *coco_remove_from_string(const char *string, const char *from, const char *to) {

  char *result, *start, *stop;

  result = coco_strdup(string);

  if (strcmp(from, "") == 0) {
    /* Remove from the start */
    start = result;
  } else
    start = strstr(result, from);

  if (strcmp(to, "") == 0) {
    /* Remove until the end */
    stop = result + strlen(result);
  } else
    stop = strstr(result, to);

  if ((start == NULL) || (stop == NULL) || (stop < start)) {
    coco_error("coco_remove_from_string(): failed to remove characters between %s and %s from string %s",
        from, to, string);
    return NULL; /* Never reached */
  }

  memmove(start, stop, strlen(stop) + 1);

  return result;
}


/**
 * @brief Returns the numbers defined by the ranges.
 *
 * Reads ranges from a string of positive ranges separated by commas. For example: "-3,5-6,8-". Returns the
 * numbers that are defined by the ranges if min and max are used as their extremes. If the ranges with open
 * beginning/end are not allowed, use 0 as min/max. The returned string has an appended 0 to mark its end.
 * A maximum of max_count values is returned. If there is a problem with one of the ranges, the parsing stops
 * and the current result is returned. The memory of the returned object needs to be freed by the caller.
 */
static size_t *coco_string_parse_ranges(const char *string,
                                        const size_t min,
                                        const size_t max,
                                        const char *name,
                                        const size_t max_count) {

  char *ptr, *dash = NULL;
  char **ranges, **numbers;
  size_t i, j, count;
  size_t num[2];

  size_t *result;
  size_t i_result = 0;

  char *str = coco_strdup(string);

  /* Check for empty string */
  if ((str == NULL) || (strlen(str) == 0)) {
    coco_warning("coco_string_parse_ranges(): cannot parse empty ranges");
    coco_free_memory(str);
    return NULL;
  }

  ptr = str;
  /* Check for disallowed characters */
  while (*ptr != '\0') {
    if ((*ptr != '-') && (*ptr != ',') && !isdigit((unsigned char )*ptr)) {
      coco_warning("coco_string_parse_ranges(): problem parsing '%s' - cannot parse ranges with '%c'", str,
          *ptr);
      coco_free_memory(str);
      return NULL;
    } else
      ptr++;
  }

  /* Check for incorrect boundaries */
  if ((max > 0) && (min > max)) {
    coco_warning("coco_string_parse_ranges(): incorrect boundaries");
    coco_free_memory(str);
    return NULL;
  }

  result = coco_allocate_vector_size_t(max_count + 1);

  /* Split string to ranges w.r.t commas */
  ranges = coco_string_split(str, ',');
  coco_free_memory(str);

  if (ranges) {
    /* Go over the current range */
    for (i = 0; *(ranges + i); i++) {

      ptr = *(ranges + i);
      /* Count the number of '-' */
      count = 0;
      while (*ptr != '\0') {
        if (*ptr == '-') {
          if (count == 0)
            /* Remember the position of the first '-' */
            dash = ptr;
          count++;
        }
        ptr++;
      }
      /* Point again to the start of the range */
      ptr = *(ranges + i);

      /* Check for incorrect number of '-' */
      if (count > 1) {
        coco_warning("coco_string_parse_ranges(): problem parsing '%s' - too many '-'s", string);
        /* Cleanup */
        for (j = i; *(ranges + j); j++)
          coco_free_memory(*(ranges + j));
        coco_free_memory(ranges);
        if (i_result == 0) {
          coco_free_memory(result);
          return NULL;
        }
        result[i_result] = 0;
        return result;
      } else if (count == 0) {
        /* Range is in the format: n (no range) */
        num[0] = (size_t) strtol(ptr, NULL, 10);
        num[1] = num[0];
      } else {
        /* Range is in one of the following formats: n-m / -n / n- / - */

        /* Split current range to numbers w.r.t '-' */
        numbers = coco_string_split(ptr, '-');
        j = 0;
        if (numbers) {
          /* Read the numbers */
          for (j = 0; *(numbers + j); j++) {
            assert(j < 2);
            num[j] = (size_t) strtol(*(numbers + j), NULL, 10);
            coco_free_memory(*(numbers + j));
          }
        }
        coco_free_memory(numbers);

        if (j == 0) {
          /* Range is in the format - (open ends) */
          if ((min == 0) || (max == 0)) {
            coco_warning("coco_string_parse_ranges(): '%s' ranges cannot have an open ends; some ranges ignored", name);
            /* Cleanup */
            for (j = i; *(ranges + j); j++)
              coco_free_memory(*(ranges + j));
            coco_free_memory(ranges);
            if (i_result == 0) {
              coco_free_memory(result);
              return NULL;
            }
            result[i_result] = 0;
            return result;
          }
          num[0] = min;
          num[1] = max;
        } else if (j == 1) {
          if (dash - *(ranges + i) == 0) {
            /* Range is in the format -n */
            if (min == 0) {
              coco_warning("coco_string_parse_ranges(): '%s' ranges cannot have an open beginning; some ranges ignored", name);
              /* Cleanup */
              for (j = i; *(ranges + j); j++)
                coco_free_memory(*(ranges + j));
              coco_free_memory(ranges);
              if (i_result == 0) {
                coco_free_memory(result);
                return NULL;
              }
              result[i_result] = 0;
              return result;
            }
            num[1] = num[0];
            num[0] = min;
          } else {
            /* Range is in the format n- */
            if (max == 0) {
              coco_warning("coco_string_parse_ranges(): '%s' ranges cannot have an open end; some ranges ignored", name);
              /* Cleanup */
              for (j = i; *(ranges + j); j++)
                coco_free_memory(*(ranges + j));
              coco_free_memory(ranges);
              if (i_result == 0) {
                coco_free_memory(result);
                return NULL;
              }
              result[i_result] = 0;
              return result;
            }
            num[1] = max;
          }
        }
        /* if (j == 2), range is in the format n-m and there is nothing to do */
      }

      /* Make sure the boundaries are taken into account */
      if ((min > 0) && (num[0] < min)) {
        num[0] = min;
        coco_warning("coco_string_parse_ranges(): '%s' ranges adjusted to be >= %lu", name,
        		(unsigned long) min);
      }
      if ((max > 0) && (num[1] > max)) {
        num[1] = max;
        coco_warning("coco_string_parse_ranges(): '%s' ranges adjusted to be <= %lu", name,
        		(unsigned long) max);
      }
      if (num[0] > num[1]) {
        coco_warning("coco_string_parse_ranges(): '%s' ranges not within boundaries; some ranges ignored", name);
        /* Cleanup */
        for (j = i; *(ranges + j); j++)
          coco_free_memory(*(ranges + j));
        coco_free_memory(ranges);
        if (i_result == 0) {
          coco_free_memory(result);
          return NULL;
        }
        result[i_result] = 0;
        return result;
      }

      /* Write in result */
      for (j = num[0]; j <= num[1]; j++) {
        if (i_result > max_count - 1)
          break;
        result[i_result++] = j;
      }

      coco_free_memory(*(ranges + i));
      *(ranges + i) = NULL;
    }
  }

  coco_free_memory(ranges);

  if (i_result == 0) {
    coco_free_memory(result);
    return NULL;
  }

  result[i_result] = 0;
  return result;
}

/**
 * @brief Trims the given string (removes any leading and trailing spaces).
 *
 * If the string contains any leading spaces, the contents are shifted so that if it was dynamically
 * allocated, it can be still freed on the returned pointer.
 */
static char *coco_string_trim(char *string) {
	size_t i, len = 0;
	int all_whitespaces = 1;
	char *frontp = string;
	char *endp = NULL;

	if (string == NULL) {
		return NULL;
	}
	if (string[0] == '\0') {
		return string;
	}

	len = strlen(string);
	endp = string + len;

	for (i = 0; ((i < len) && all_whitespaces); i++)
		all_whitespaces = all_whitespaces && isspace(string[i]);
	if (all_whitespaces) {
	  string[0] = '\0';
		return string;
	}

	/* Move the front and back pointers to address the first non-whitespace characters from each end. */
	while (isspace((unsigned char) *frontp)) {
		++frontp;
	}
	if (endp != frontp) {
		while (isspace((unsigned char) *(--endp)) && endp != frontp) {
		}
	}

	if (string + len - 1 != endp)
		*(endp + 1) = '\0';
	else if (frontp != string && endp == frontp)
		*string = '\0';

	/* Shift the string. Note the reuse of endp to mean the front of the string buffer now. */
	endp = string;
	if (frontp != string) {
		while (*frontp) {
			*endp++ = *frontp++;
		}
		*endp = '\0';
	}

	return string;
}

