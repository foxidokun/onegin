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
    #define big_t __m128i
#else
    #define big_t uint64_t
#endif

void swap (void *a, void *b, size_t size)
{
    assert (a != NULL && "pointer can't be NULL");
    assert (b != NULL && "pointer can't be NULL");

    big_t *c_a_big = (big_t *) a;
    big_t *c_b_big = (big_t *) b;
    big_t  tmp_big = {0, 0};

    while (size > sizeof (big_t))
    {
        tmp_big  = *c_a_big;
        *c_a_big = *c_b_big;
        *c_b_big =  tmp_big;

        c_a_big++;
        c_b_big++;
        size -= sizeof (big_t);
    }

    char *c_a = (char *) c_a_big;
    char *c_b = (char *) c_b_big;
    char  tmp = 0;

    do
    {
        tmp  = *c_a;
        *c_a = *c_b;
        *c_b =  tmp;

        c_a++;
        c_b++;
        size--;
    } while (size > 0);

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
