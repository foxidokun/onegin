#ifndef BITS_H
#define BITS_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/**
 * @brief      Check nth bit in byte
 *
 * @param[in]  bitmap  Byte
 * @param[in]  index   Bit index
 *
 * @return     True or false
 */
static inline int check_bit (uint8_t byte, char index)
{
    assert (index >= 0 && index < 8 && "index must be in [0, 8) range");

    return (1<<index) & byte;
}

/**
 * @brief      Set nth bit in byte to (bool) value
 *
 * @param      byte   Byte
 * @param[in]  index  The index
 * @param[in]  value  Bool value
 */
static inline void set_bit (uint8_t *byte, char index, char value)
{
    assert (index >= 0    && "index can't be less than zero");
    assert (index  < 8    && "index can't be greater than zero");
    assert (byte  != NULL && "pointer can't be NULL");

    if (value) *byte |= (uint8_t) (1<<index);
    else       *byte =  (uint8_t) ~((~*byte) | (1<<index));
}

#endif