#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdint.h>

/// Minimal length of valid string
static const int MIN_LEN = 8;
/// Minimal percantage of alpha characters in valid string
static const double MIN_ALPHA_PERSENTAGE = 0.8;

/// Number of lines in with the same rhyme
static const int BLOCK_SIZE = 4;

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

#endif //GENERATOR_H