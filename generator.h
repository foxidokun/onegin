#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdint.h>
#include <stdlib.h>

/// Minimal length of valid string
static const int MIN_LEN = 8;
/// Minimal percantage of alpha characters in valid string
static const double MIN_ALPHA_PERSENTAGE = 0.8;

/// Number of lines in with the same rhyme
static const int BLOCK_SIZE = 4;

/// Hashmap size in struct chain. Choosed for 32MiB hashmap size
static const unsigned int HASHMAP_SIZE   = 4194304;
/// Number of possible chars in stat::seq and stat::prop
static const unsigned int CHAR_CNT       = 256;
/// Max len of prefix in stat::prefix
static const unsigned int MAX_PREFIX_LEN = 3;

//---------------------------------------------------------------------------------------------------------

/**
 * @brief Markov chain prefix stats
 */
struct stat
{
    unsigned int prop[CHAR_CNT];    /// Array of char counters 
    unsigned int total;             /// Sum of counters
    char       *prefix;             /// Prefix for this chars
    unsigned int prefix_len;        /// Prefix len
    stat         *next;             /// Ponter to next stat in hashmap
};

/**
 * @brief      Markov chain hashmap
 */
struct chain
{
    stat **hashmap;
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
 * @brief      Allocates chain
 *
 * @return     Pointer to chain
 */
chain *create_chain ();

/**
 * @brief      Free memory, used by chain
 *
 * @param      ch    chain3
 */
void free_chain (chain *ch);

/**
 * @brief      Creates statistics from text and writes them into chain
 *
 * @param[in]  text   Text
 * @param      chain  Chain
 */
void collect_stats (const text *text, chain *chain);

/**
 * @brief      Generate text with markov chain and write it into buffer
 *
 * @param[in]  chain     Chain
 * @param      buf       Buffer
 * @param[in]  buf_size  Buffer size
 * @param      seed      Seed
 */
void generate_text (const chain *chain, char *buf, size_t buf_size, const char *seed);

#endif //GENERATOR_H