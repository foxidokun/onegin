#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

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
 */
void alpha_file_lines_sort (struct text *file);

/**
 * @brief      Sort lines by their reversed content
 */
void rev_alpha_file_lines_sort (struct text *file);

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
 * @brief      isalpha for CP1251 encoding
 *
 * @param[in]  c     char
 *
 * @return     bool value
 */
int cp1251_isalpha (char c);

/**
 * @brief      Skip non-alpha (CP1251) characters
 *
 * @param[in]  str   Null-terminated string
 *
 * @return     Pointer to first alpha character
 */
char *skip_nalpha_cp1251 (const char *str);

/**
 * @brief      Skip non-alpha (CP1251) characters in reverse direction
 *
 * @param[in]  str     The string
 * @param[in]  index   Index of last character (len - 1)
 *
 * @return     Index of last alpha character
 */
size_t rev_skip_nalpha_cp1251 (const char *str, size_t index);

#endif