#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <emmintrin.h>
#include <stdint.h>
#include "onegin.h"
#include "sort.h"

/// Encoding ranges
static const int  ENG_UP_MIN_VAL = 'A';
static const int  ENG_UP_MAX_VAL = 'Z';
static const int ENG_LOW_MIN_VAL = 'a';
static const int ENG_LOW_MAX_VAL = 'z';
static const int     RUS_MIN_VAL = 192;
static const int     RUS_MAX_VAL = 255;

/// Minimal length of valid string
static const int MIN_LEN = 8;
/// Minimal percantage of alpha characters in valid string
static const double MIN_ALPHA_PERSENTAGE = 0.8;

/// Number of lines in with the same rhyme
static const int BLOCK_SIZE = 4;

//---------------------------------------------------------------------------------------------------------

static int check_bit (uint8_t byte, char index);
static void set_bit (uint8_t *byte, char index, char value);

static unsigned int min (unsigned int a, unsigned int b);
static unsigned int max (unsigned int a, unsigned int b);

static long int find_candidate (const struct text *text, unsigned int from,
                                unsigned int to, uint8_t *used);

//---------------------------------------------------------------------------------------------------------

#define __UNWRAP(expr) { if ((expr) == ERROR) { return ERROR; } }

//---------------------------------------------------------------------------------------------------------

void alpha_file_lines_sort (struct text *lines, void sort_func (void *, size_t, size_t, comparator_f))
{
    assert (lines != NULL && "pointer can't be NULL");

    sort_func (lines->lines, lines->n_lines, sizeof (struct line), alpha_linecmp);
}

void rev_alpha_file_lines_sort (struct text *lines, void sort_func (void *, size_t, size_t, comparator_f))
{
    assert (lines != NULL && "pointer can't be NULL");

    sort_func (lines->lines, lines->n_lines, sizeof (struct line), rev_alpha_linecmp);
}

int alpha_linecmp (const void *lhs, const void *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    const struct line *lhs_cast = (const struct line *) lhs;
    const struct line *rhs_cast = (const struct line *) rhs;

    return alpha_strcmp (lhs_cast->content, rhs_cast->content);
}

int rev_alpha_linecmp (const void *lhs, const void *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    const struct line *lhs_cast = (const struct line *) lhs;
    const struct line *rhs_cast = (const struct line *) rhs;

    return rev_alpha_strcmp (lhs_cast->content, lhs_cast->len, rhs_cast->content, rhs_cast->len);
}

char *skip_nalpha_cp1251 (char *str)
{
    assert (str != NULL && "pointer can't be NULL");

    return const_cast<char*>(skip_nalpha_cp1251((const char*) str));
}

const char *skip_nalpha_cp1251 (const char *str)
{
    assert (str != NULL && "pointer can't be NULL");

    while (str[0] != '\0' && !cp1251_isalpha (str[0])) str++;

    return str;
}

size_t rev_skip_nalpha_cp1251 (const char *str, size_t len)
{
    assert (str != NULL && "pointer can't be NULL");

    while (len > 0 && !cp1251_isalpha (str[len])) len--;

    return len;
}

int alpha_strcmp (const char *lhs, const char *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    lhs = skip_nalpha_cp1251 (lhs);
    rhs = skip_nalpha_cp1251 (rhs);

    while (
           lhs[0] != '\0' &&
           rhs[0] != '\0' &&
           lhs[0] == rhs[0]
           )
    {
        lhs++;
        rhs++;

        lhs = skip_nalpha_cp1251 (lhs);
        rhs = skip_nalpha_cp1251 (rhs);
    }

    return chrcmp (lhs[0], rhs[0]);
}

int rev_alpha_strcmp (const char *lhs, size_t l_len, const char *rhs, size_t r_len)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    // Max index is len - 1 
    l_len--; 
    r_len--;

    l_len = rev_skip_nalpha_cp1251 (lhs, l_len);
    r_len = rev_skip_nalpha_cp1251 (rhs, r_len);

    while (
           l_len > 0 &&
           r_len > 0 &&
           lhs[l_len] == rhs[r_len]
           )
    {
        l_len--;
        r_len--;

        l_len = rev_skip_nalpha_cp1251 (lhs, l_len);
        r_len = rev_skip_nalpha_cp1251 (rhs, r_len);
    }

    return chrcmp (lhs[l_len], rhs[r_len]);
}

int chrcmp (char lhs, char rhs)
{
    if      (lhs < rhs) return -1;
    else if (lhs > rhs) return +1;
    else                return +0;
}

int cp1251_isalpha (char c)
{
    unsigned char u_c = (unsigned char) c;   

    return (u_c >=     RUS_MIN_VAL && u_c <=     RUS_MAX_VAL)
        || (u_c >=  ENG_UP_MIN_VAL && u_c <=  ENG_UP_MAX_VAL)
        || (u_c >= ENG_LOW_MIN_VAL && u_c <= ENG_LOW_MAX_VAL);
}

#ifdef __SSE2__
    #define _big_t __m128i
    #define _tmp_init_val {0, 0}
#else
    #define _big_t uint64_t
    #define _tmp_init_val 0
#endif

void swap (void *a, void *b, size_t size)
{
    assert (a != NULL && "pointer can't be NULL");
    assert (b != NULL && "pointer can't be NULL");

    _big_t *c_a_big = (_big_t *) a;
    _big_t *c_b_big = (_big_t *) b;
    _big_t  tmp_big = _tmp_init_val;

    while (size >= sizeof (_big_t))
    {
        tmp_big  = *c_a_big;
        *c_a_big = *c_b_big;
        *c_b_big =  tmp_big;

        c_a_big++;
        c_b_big++;
        size -= sizeof (_big_t);
    }

    char *c_a = (char *) c_a_big;
    char *c_b = (char *) c_b_big;
    char  tmp = 0;

    while (size > 0)
    {
        tmp  = *c_a;
        *c_a = *c_b;
        *c_b =  tmp;

        c_a++;
        c_b++;
        size--;
    }

}

void cust_qsort (void* base, size_t count, size_t size, comparator_f comp) 
{
    assert (base != NULL && "pointer can't be NULL");

    char *base_p = (char *) base;
    size_t lo    = 0;
    size_t hi    = count - 1;
    size_t pi    = count / 2;

    while (hi > lo)
    {
        while (comp (base_p + lo*size, base_p + pi*size) < 0 && lo < count) lo++;
        while (comp (base_p + pi*size, base_p + hi*size) < 0 && hi > 0)     hi--;

        if (lo <= hi)
        {
            swap (base_p + lo*size, base_p + hi*size, size);

            if      (pi == lo)  pi = hi;
            else if (pi == hi)  pi = lo;

            lo++;
            hi--;
         }
    }

    if (hi > 0)
    {
        cust_qsort (base_p, hi + 1, size, comp);
    }

    if (lo < count - 1)
    {
        cust_qsort (base_p + lo*size, count - lo, size, comp);
    }
}

int poem_generator (const struct text *text, char **buf, unsigned int buf_size,
                        unsigned char range)
{
    assert (text != NULL     && "pointer can't be NULL");
    assert (buf  != NULL     && "pointer can't be NULL");
    assert (buf_size >= 2    && "can't construct poem from less than two lines");

    line *lines          = text->lines;
    unsigned int n_lines = text->n_lines;

    // Bool bits
    uint8_t *used        = (uint8_t *) calloc (n_lines/8 + 1, sizeof (uint8_t));

    unsigned int pos[2]  = {};

    long int pos_tmp = 0;

    unsigned int cand_num    = 0;
    long int cand_num_tmp    = 0;
    unsigned char parity     = 0;


    for (size_t n = 0; n < buf_size; ++n)
    {
        if (n % BLOCK_SIZE == 0)
        {
            __UNWRAP (pos_tmp = find_candidate (text, 0, n_lines, used));
            pos[0] = (unsigned int) pos_tmp;
            __UNWRAP (pos_tmp = find_candidate (text, 0, n_lines, used));
            pos[1] = (unsigned int) pos_tmp;
        }

        parity = n % 2;

        // Find not used variables near pos[parity]
        cand_num_tmp = find_candidate (text, max (0, pos[parity] - range),
                        min (n_lines, pos[parity] + range + 1), used);

        if (cand_num_tmp == ERROR)
        {   
            return ERROR;
        }

        cand_num = (unsigned int) cand_num_tmp;

        // Select random candidate
        pos[parity] = cand_num;
        buf[n] = lines[cand_num].content;
    }

    free (used);

    return 0;
}

/**
 * @brief      Finds a good enough random string, that is not used yet and mark it as used
 *
 * @param[in]  text       Text
 * @param[in]  from       Min index
 * @param[in]  to         Max index
 * @param      used       Used indicator (from poem_generator)
 * @param[in]  cand_size  Max number of candidates
 *
 * @return     (unsigned int) candidate index or ERROR on range
 */
static long int find_candidate (const struct text *text, unsigned int from,
                                unsigned int to, uint8_t *used)
{
    assert (text != NULL       && "pointer can't be NULL");
    assert (from < to          && "range can't be empty");

    unsigned int *cand_list = (unsigned int *) calloc (from - to, sizeof (int));    

    line *lines           = text->lines;
    unsigned int cand_num = 0;
    size_t       line_len = 0;
    size_t      alpha_cnt = 0;

    for (unsigned int n = from; n < to; ++n)
    {
        if (!check_bit (used[n/8], n%8))
        {
            //Skip not big enough lines
            line_len = strlen (lines[n].content);
            if (line_len < MIN_LEN) continue;

            //Skip lines from non alpha characters mostly
            alpha_cnt = 0;

            for (size_t i = 0; i < line_len; ++i)
            {
                if (cp1251_isalpha(lines[n].content[i])) alpha_cnt++;
            }

            if (((long double) alpha_cnt) / line_len < MIN_ALPHA_PERSENTAGE) continue;

            cand_list[cand_num] = n;
            cand_num++;
        }
    }

    if (cand_num == 0)
    {
        return ERROR;
    }

    unsigned int cand = cand_list[(unsigned int) rand() % cand_num];

    set_bit (used + (cand/8), cand%8, 1);

    free (cand_list);
    
    return cand;
}

/**
 * @brief      Check nth bit in byte
 *
 * @param[in]  bitmap  Byte
 * @param[in]  index   Bit index
 *
 * @return     True or false
 */
static int check_bit (uint8_t byte, char index)
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
static void set_bit (uint8_t *byte, char index, char value)
{
    assert (index >= 0    && "index can't be less than zero");
    assert (index  < 8    && "index can't be greater than zero");
    assert (byte  != NULL && "pointer can't be NULL");

    if (value) *byte |= (uint8_t) (1<<index);
    else       *byte =  (uint8_t) ~((~*byte) | (1<<index));
}

static unsigned int max (unsigned int a, unsigned int b)
{
    return (a > b) ? a : b;
}

static unsigned int min (unsigned int a, unsigned int b)
{
    return (a < b) ? a : b;
}