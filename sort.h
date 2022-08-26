#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

/**
 * @brief      Compare given strings only by alpha characters
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @note Strings are '\n' terminated
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
void alpha_file_lines_sort (struct file *file);

/**
 * @brief      Sort lines by their reversed content
 */
void rev_alpha_file_lines_sort (struct file *file);

/**
 * @brief      Compare lines by their numbers
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
int num_linecmp (const struct line *lhs, const struct line *rhs);

/**
 * @brief      Compare lines by their content
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
int alpha_linecmp (const struct line *lhs, const struct line *rhs);

/**
 * @brief      Compare lines by their reversed content
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
int rev_alpha_linecmp (const struct line *lhs, const struct line *rhs);

/**
 * @brief      Compare reversed given strings only by alpha characters
 *
 * @note Strings are '\n' terminated
 * 
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
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

#endif