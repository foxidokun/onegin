#ifndef ONEGIN_H
#define ONEGIN_H
#include <stdlib.h>

const int ERROR = -1;

/**
 * @brief      File content 
 */
struct text
{
    char *content       = NULL;  /// Content of file, but '\n' are replaced with '\0'
    struct line* lines  = NULL;  /// Array of lines, but line::content is pointer to file_lines::content
    unsigned int cnt    = 0;     /// Number of lines
    size_t content_size = 0;     /// Size of content
};

/**
 * @brief      Store one line and it's len
 */
struct line
{
    char  *content  = NULL;   /// Line content ('\0' terminated)
    size_t     len  = 0;      /// Line length
};


#endif