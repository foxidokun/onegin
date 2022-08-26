#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "onegin.h"
#include "sort.h"

/// Encoding ranges
static const int  ENG_UP_MIN_VAL =  65;
static const int  ENG_UP_MAX_VAL =  90;
static const int ENG_LOW_MIN_VAL =  97;
static const int ENG_LOW_MAX_VAL = 122;
static const int     RUS_MIN_VAL = 192;
static const int     RUS_MAX_VAL = 255;


void alpha_file_lines_sort (struct file *lines)
{
    assert (lines != NULL && "pointer can't be NULL");

    qsort(lines->lines, lines->cnt, sizeof (struct line), alpha_linecmp);
}

void rev_alpha_file_lines_sort (struct file *lines)
{
    assert (lines != NULL && "pointer can't be NULL");

    qsort(lines->lines, lines->cnt, sizeof (struct line), rev_alpha_linecmp);
}

int alpha_linecmp (const void *lhs, const void *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    const struct line *lhs_cast = (const struct line *) lhs;
    const struct line *rhs_cast = (const struct line *) rhs;

    return alpha_strcmp(lhs_cast->content, rhs_cast->content);
}

int rev_alpha_linecmp (const void *lhs, const void *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    const struct line *lhs_cast = (const struct line *) lhs;
    const struct line *rhs_cast = (const struct line *) rhs;

    return rev_alpha_strcmp(lhs_cast->content, lhs_cast->len, rhs_cast->content, rhs_cast->len);
}

int alpha_strcmp (const char *lhs, const char *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    while (lhs[0] != '\n' && !cp1251_isalpha (lhs[0])) lhs++;
    while (rhs[0] != '\n' && !cp1251_isalpha (rhs[0])) rhs++;

    while (
           lhs[0] != '\n' &&
           rhs[0] != '\n' &&
           lhs[0] == rhs[0]
           )
    {
        lhs++;
        rhs++;

        while (lhs[0] != '\n' && !cp1251_isalpha (lhs[0])) lhs++;
        while (rhs[0] != '\n' && !cp1251_isalpha (rhs[0])) rhs++;
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

    while (l_len > 0 && !cp1251_isalpha (lhs[l_len])) l_len--;
    while (r_len > 0 && !cp1251_isalpha (rhs[r_len])) r_len--;

    while (
           l_len > 0 &&
           r_len > 0 &&
           lhs[l_len] == rhs[r_len]
           )
    {
        l_len--;
        r_len--;

        while (l_len > 0 && !cp1251_isalpha (lhs[l_len])) l_len--;
        while (r_len > 0 && !cp1251_isalpha (rhs[r_len])) r_len--;
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