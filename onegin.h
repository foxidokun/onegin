#ifndef ONEGIN_H
#define ONEGIN_H

/**
 * @brief      File lines number and content 
 */
struct file_lines
{
    struct line* lines;     /// Array of lines
    unsigned int cnt;       /// Number of lines
    unsigned int alloc_cnt; /// Allocated lines size
};

/**
 * @brief      Store one line and it's number
 */
struct line
{
    char        *content;  ///Line content
    unsigned int number;   ///Line number
};


#endif