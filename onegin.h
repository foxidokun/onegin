#ifndef ONEGIN_H
#define ONEGIN_H
#include <stdlib.h>

/**
 * @brief      File content 
 */
struct text
{
    char *content;          /// Content of file, but '\n' are replaced with '\0'
    struct line* lines;     /// Array of lines, but line::content is pointer to file_lines::content
    unsigned int cnt;       /// Number of lines
    size_t content_size;    /// Size of content
};

/**
 * @brief      Store one line and it's len
 */
struct line
{
    char  *content;   /// Line content ('\0' terminated)
    size_t     len;   /// Line length
};


#endif