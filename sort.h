#ifndef SORT_H
#define SORT_H

#include <stdlib.h>
#include <stdint.h>

typedef int (*comparator_f)(const void*, const void*);

/**
 * @brief      Compare given strings only by alpha characters
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
int alpha_strcmp (const char *lhs, const char *rhs);

/**
 * @brief      Compare two given chars
 *
 * @note Considers that smaller characters have smalle char codes.
 * 
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
int chrcmp (char lhs, char rhs);

/**
 * @brief      Sort lines by their content
 * 
 * @param[in]   lines       Text to sort
 * @param       sort_func   qsort API compatable sort function
 */
void alpha_file_lines_sort (struct text *lines, void sort_func (void *, size_t, size_t, comparator_f));


/**
 * @brief      Sort lines by their reversed content
 * 
 * @param[in]   lines       Text to sort
 * @param       sort_func   qsort API compatable sort function
 */
void rev_alpha_file_lines_sort (struct text *lines, void sort_func (void *, size_t, size_t, comparator_f));

/**
 * @brief      Compare lines by their content
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
int alpha_linecmp (const void *lhs, const void *rhs);

/**
 * @brief      Compare lines by their reversed content
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
int rev_alpha_linecmp (const void *lhs, const void *rhs);

/**
 * @brief      Compare reversed given strings only by alpha characters
 * 
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 * @param[in]  l_len Length of lhs
 * @param[in]  r_len Length of rhs
 *
 * @return     as in strcmp
 */
int rev_alpha_strcmp (const char *lhs, size_t l_len, const char *rhs, size_t r_len);

/**
 * @brief      Skip non-alpha characters
 *
 * @param[in]  str   Null-terminated string
 *
 * @return     Pointer to first alpha character
 */
const char *skip_nalpha (const char *str);
char       *skip_nalpha (char *str);


/**
 * @brief      Skip non-alpha characters in reverse direction
 *
 * @param[in]  str     The string
 * @param[in]  index   Index of last character (len - 1)
 *
 * @return     Index of last alpha character
 */
size_t rev_skip_nalpha (const char *str, size_t index);

/// @brief custom qsort with standart qsort API-compatable realisation
void cust_qsort (void *base, size_t count, size_t size, comparator_f comp);

/**
 * @brief      Swap two object pointed by a and b with `size` size
 *
 * @param      a     
 * @param      b     
 * @param[in]  size  Objects size
 */
void swap (void *a, void *b, size_t size);

#endif