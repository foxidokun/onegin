#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "onegin.h"
#include "sort.h"

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
    int lhs_n = lhs->number;
    int rhs_n = rhs->number;

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

    while (
           lhs[0] != '\0' &&
           rhs[0] != '\0' &&
           lhs[0] == rhs[0]
           )
    {
        while (!isalpha (lhs[0])) lhs++;
        while (!isalpha (rhs[0])) rhs++;
    }

    return chrcmp (lhs[0], rhs[0]);
}

int rev_alpha_strcmp (const char *lhs, const char *rhs)
{
    assert (lhs != NULL && "pointer can't be NULL");
    assert (rhs != NULL && "pointer can't be NULL");

    int l_cur = strlen (lhs);
    int r_cur = strlen (rhs);

    while (
           lhs[l_cur] != '\0' &&
           rhs[r_cur] != '\0' &&
           lhs[l_cur] == rhs[r_cur]
           )
    {
        while (!isalpha (lhs[l_cur])) l_cur--;
        while (!isalpha (rhs[r_cur])) r_cur--;
    }

    return chrcmp (lhs[l_cur], rhs[r_cur]);
}

int chrcmp (char lhs, char rhs)
{
    if      (lhs < rhs) return -1;
    else if (lhs > rhs) return +1;
    else                return +0;
}