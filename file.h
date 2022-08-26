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
struct text *read_text (FILE *stream);

/**
 * @brief      Free memory, used by given file struct
 */
void free_text (struct text *f_lines);

/**
 * @brief      Write lines into given stream
 *
 * @param      text    Text
 * @param      stream  The stream write lines to
 *
 * @return     Non-zero value on error
 */
int write_lines (const struct text *text, FILE *stream);

/**
 * @brief      Write original buffer into stream
 *
 * @param[in]  text    Text 
 * @param      stream  The stream write lines to
 *
 * @return     Non-zero value on error
 */
int write_buf (const struct text *text, FILE *stream);

/**
 * @brief      Return stream size in bytes
 *
 * @param      stream  Stream
 *
 * @return     File size in bytes or -1 if error occured
 */
ssize_t file_size (FILE *stream);

/**
 * @brief      Counts number of lines in text
 *
 * @param[in]  text      Null-terminated string
 *
 * @return     Number of lines.
 */
unsigned int count_lines (const char *text);

/**
 * @brief      Point text::lines to line first characters in text::content and change '\n' to '\0' in text::content
 *
 * @param      text  text
 */
void create_index (struct text *text);

#endif