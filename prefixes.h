#ifndef PREFIXES_H
#define PREFIXES_H

#include <stdlib.h>

struct prefixes
{
    char *buf;
    size_t max_len;
};

/**
 * @brief      Creates prefixes.
 *
 * @param[in]  max_len      The maximum length
 * @param      init_string  The initialize string witl len >= max_len
 *
 * @return     pointer to allocated memory or NULL in case of OOM
 */
prefixes *create_prefixes (size_t max_len, char *init_string);

/**
 * @brief      Free mempry pccupied by prefixes
 *
 * @param      pr    Prefixes
 */
void free_prefixes (prefixes *pr);

/**
 * @brief      Update prefixes with next characters
 *
 * @param      pr        { parameter_description }
 * @param[in]  next_chr  The next character
 */
void update_prefixes (prefixes *pr, char next_chr);

const char *get_prefix (const prefixes *pr, size_t len);

#endif