#ifndef LOG_H
#define LOG_H

#include <stdio.h>

enum class log
{
    DBG = 1,
    INF = 2,
    WRN = 3,
    ERR = 4,
};

#ifndef __LOG_CPP
extern log __LOG_LEVEL;
extern FILE *__LOG_OUT_STREAM;
#endif

#define R       "\033[91m"
#define G       "\033[92m"
#define Cyan    "\033[96m"
#define Y       "\033[93m"
#define D       "\033[39m"


#define log(lvl, ...)                                                   \
{                                                                       \
    if (lvl >= __LOG_LEVEL)                                             \
    {                                                                   \
        if      (lvl == log::DBG) { printf ("DEBUG"); }                 \
        else if (lvl == log::INF) { printf (Cyan "INFO " D); }          \
        else if (lvl == log::WRN) { printf (Y "WARN " D); }             \
        else if (lvl == log::ERR) { printf (R "ERROR" D); }             \
                                                                        \
        fprintf (__LOG_OUT_STREAM, " [" __FILE__ ":%d] ", __LINE__);    \
        fprintf (__LOG_OUT_STREAM, __VA_ARGS__);                        \
        fputc   ('\n', __LOG_OUT_STREAM);                               \
    }                                                                   \
}                                                       

/**
 * @brief      Sets the log level.
 *
 * @param[in]  level  The level
 */

void set_log_level (log level);

/**
 * @brief      Sets the output log stream.
 *
 * @param      stream  Stream
 */
void set_log_stream (FILE *stream);

#endif //LOG_H