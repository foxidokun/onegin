#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <emmintrin.h>
#include <stdint.h>
#include "onegin.h"
#include "sort.h"

//---------------------------------------------------------------------------------------------------------
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

char *skip_nalpha (char *str)
{
    assert (str != NULL && "pointer can't be NULL");

    return const_cast<char*>(skip_nalpha((const char*) str));
}

const char *skip_nalpha (const char *str)
{
    assert (str != NULL && "pointer can't be NULL");

    while (str[0] != '\0' && !isalpha (str[0])) str++;

    return str;
}

size_t rev_skip_nalpha (const char *str, size_t len)
{
    assert (str != NULL && "pointer can't be NULL");

    while (len > 0 && !isalpha (str[len])) len--;

    return len;
}

int alpha_strcmp (const char *lhs, const char *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    lhs = skip_nalpha (lhs);
    rhs = skip_nalpha (rhs);

    while (
           lhs[0] != '\0' &&
           rhs[0] != '\0' &&
           lhs[0] == rhs[0]
           )
    {
        lhs++;
        rhs++;

        lhs = skip_nalpha (lhs);
        rhs = skip_nalpha (rhs);
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

    l_len = rev_skip_nalpha (lhs, l_len);
    r_len = rev_skip_nalpha (rhs, r_len);

    while (
           l_len > 0 &&
           r_len > 0 &&
           lhs[l_len] == rhs[r_len]
           )
    {
        l_len--;
        r_len--;

        l_len = rev_skip_nalpha (lhs, l_len);
        r_len = rev_skip_nalpha (rhs, r_len);
    }

    return chrcmp (lhs[l_len], rhs[r_len]);
}

int chrcmp (char lhs, char rhs)
{
    return lhs - rhs;
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

    char *const base_p = (char *) base;
    // Small array optimisations

    if      (count == 1) { return; }
    else if (count == 2 && comp (base_p, base_p + size) > 0)
    {
        swap (base_p + size, base_p + size, size);
    }
    else if (count == 3)
    {
        if (comp (base_p, base_p + size) > 0)
        {   swap (base_p, base_p + size, size);
        }

        if (comp (base_p + size, base_p + 2*size) > 0)
        {   swap (base_p + size, base_p + 2*size, size);

            if (comp (base_p, base_p + size) > 0)
            {   swap (base_p, base_p + size, size);
            }
        }
    }

    // Regular algorithm

    size_t lo          = 0;
    size_t hi          = count - 1;
    size_t pi          = count / 2;

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
