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


void num_file_lines_sort (struct file_lines *lines)
{
    qsort(lines->lines, lines->cnt, sizeof (struct line), (int (*)(const void *, const void *)) num_linecmp);
}

void alpha_file_lines_sort (struct file_lines *lines)
{
    qsort(lines->lines, lines->cnt, sizeof (struct line), (int (*)(const void *, const void *)) alpha_linecmp);
}

void rev_alpha_file_lines_sort (struct file_lines *lines)
{
    qsort(lines->lines, lines->cnt, sizeof (struct line), (int (*)(const void *, const void *)) rev_alpha_linecmp);
}

int num_linecmp (const struct line *lhs, const struct line *rhs)
{
    unsigned int lhs_n = lhs->number;
    unsigned int rhs_n = rhs->number;

    if      (lhs_n < rhs_n) return -1;
    else if (lhs_n > rhs_n) return +1;
    else                    return +0;
}

int alpha_linecmp (const struct line *lhs, const struct line *rhs)
{
    return alpha_strcmp(lhs->content, rhs->content);
}

int rev_alpha_linecmp (const struct line *lhs, const struct line *rhs)
{
    return rev_alpha_strcmp(lhs->content, rhs->content);
}

int alpha_strcmp (const char *lhs, const char *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    while (lhs[0] != '\0' && !cp1251_isalpha (lhs[0])) lhs++;
    while (rhs[0] != '\0' && !cp1251_isalpha (rhs[0])) rhs++;

    while (
           lhs[0] != '\0' &&
           rhs[0] != '\0' &&
           lhs[0] == rhs[0]
           )
    {
        lhs++;
        rhs++;
        
        while (lhs[0] != '\0' && !cp1251_isalpha (lhs[0])) lhs++;
        while (rhs[0] != '\0' && !cp1251_isalpha (rhs[0])) rhs++;
    }

    return chrcmp (lhs[0], rhs[0]);
}

int rev_alpha_strcmp (const char *lhs, const char *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    size_t l_cur = strlen (lhs);
    size_t r_cur = strlen (rhs);

    while (l_cur > 0 && !cp1251_isalpha (lhs[l_cur])) l_cur--;
    while (r_cur > 0 && !cp1251_isalpha (rhs[r_cur])) r_cur--;

    while (
           l_cur > 0 &&
           r_cur > 0 &&
           lhs[l_cur] == rhs[r_cur]
           )
    {
        l_cur--;
        r_cur--;

        while (l_cur > 0 && !cp1251_isalpha (lhs[l_cur])) l_cur--;
        while (r_cur > 0 && !cp1251_isalpha (rhs[r_cur])) r_cur--;
    }

    return chrcmp (lhs[l_cur], rhs[r_cur]);
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