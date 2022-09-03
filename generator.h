#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdint.h>
#include <stdlib.h>
#include "hashmap.h"

/// Minimal length of valid string
static const int MIN_LEN = 8;
/// Minimal percantage of alpha characters in valid string
static const double MIN_ALPHA_PERSENTAGE = 0.8;

/// Number of lines in with the same rhyme
static const int BLOCK_SIZE = 4;

/// Initial number of prefixes in markov chain
static const int NPREFIXES = 256;

//---------------------------------------------------------------------------------------------------------

/// Markov chain struct
struct chain
{
    hashmap *map;               /// Hashmap "prefix -> stat *"
    size_t max_prefix_len;      /// Max prefix lenght in collected stats
};

struct stat
{
    unsigned long int total;         /// Total counter
    unsigned      int char_cnt[256]; /// Per-char counter
};

//---------------------------------------------------------------------------------------------------------

/**
 * @brief      Generates poem like text
 *
 * @note Requires initialized rand
 * 
 * @note buf_size must be greater than one
 *
 * @param[in]  text            text struct generate poem from
 * @param      buf             Buffer for char *, pointing to poem lines
 * @param[in]  buf_size        The buffer size
 * @param[in]  range           Max distance between rhyming lines in reverse sort output
 *
 * @return     Non-zero value on error
 */
int poem_generator (const struct text *text, char **buf, unsigned int buf_size,
                        unsigned char range);

/**
 * @brief      Creates a Markov chain.
 *
 * @param[in]  max_prefix_len  The maximum prefix length
 *
 * @return     Pointer to allocated memory or NULL in case of OOM
 */
chain *create_chain (size_t max_prefix_len);

/**
 * @brief      Free memory occupied by chain struct
 *
 * @param      ch    Chain
 */
void free_chain (chain *ch);

/**
 * @brief      Collect Markov chain stats from given text
 *
 * @param[in]  text  Text
 * @param      ch    Chain
 *
 * @return     Non-zero value on error
 */
int collect_stats (const text *text, chain *ch);

#endif //GENERATOR_H