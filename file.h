#ifndef FILE_H
#define FILE_H
#include <stdio.h>
#include "onegin.h"

/**
 * @brief              Read stream and create file struct
 *
 * @param[in]  stream  The stream to read data from
 *
 * @return             File
 */
struct file *read_file (FILE *stream);

/**
 * @brief      Free memory, used by given file struct
 */
void free_file (struct file *f_lines);

/**
 * @brief      Write lines into given stream
 *
 * @param      file    File
 * @param      stream  The stream write lines to
 *
 * @return     Non-zero value on error
 */
int write_lines (const struct file *file, FILE *stream);

/**
 * @brief      Write original buffer into stream
 *
 * @param[in]  file    File 
 * @param      stream  The stream write lines to
 *
 * @return     Non-zero value on error
 */
int write_buf (const struct file *file, FILE *stream);

#endif