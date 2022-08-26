#ifndef FILE_H
#define FILE_H
#include <stdio.h>
#include "onegin.h"

/**
 * @brief              Reads lines from stream
 *
 * @param[in]  stream  The stream to read data from
 *
 * @return             File lines in file_lines struct
 */
struct file_lines *read_lines (FILE *stream);

/**
 * @brief      Insert line with given content and number into file_lines struct
 *
 * @param      lines         Pointer to file lines to insert line to
 * @param[in]  content       Line content
 * @param[in]  number        Line number
 *
 * @return     Non-zero value on error
 */
int insert_line (struct file_lines **lines, char *content, unsigned int number); 

/**
 * @brief      Free memory, used by given file_lines struct
 */
void free_file_lines (struct file_lines *f_lines);

/**
 * @brief      Write lines into given stream
 *
 * @param      lines   File lines
 * @param      stream  The stream write lines to
 *
 * @return     Non-zero value on error
 */
int write_lines (const struct file_lines *lines, FILE *stream);

/**
 * @brief      Write original buffer into stream
 *
 * @param[in]  lines   File lines
 * @param      stream  The stream write lines to
 *
 * @return     Non-zero value on error
 */
int write_buf (const struct file_lines *lines, FILE *stream);

#endif