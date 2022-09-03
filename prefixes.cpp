#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "onegin.h"
#include "prefixes.h"


prefixes *create_prefixes (size_t max_len, const char *init_string)
{
    assert (init_string != NULL && "pointer can't be NULL");
    assert (strlen (init_string) >= max_len && "Init string too small");

    prefixes *pr = (prefixes *) calloc (1, sizeof (prefixes));
    _UNWRAP_NULL (pr);

    pr->max_len = max_len;
    pr->buf     = (char *) calloc (max_len+1, sizeof (char));
    _UNWRAP_NULL (pr->buf);

    size_t len = strlen (init_string);
    strncpy (pr->buf, init_string + (len-max_len), max_len+1);

    return pr;
}

void free_prefixes (prefixes *pr)
{
    assert (pr != NULL && "pointer can't be NULL");

    free (pr->buf);
    free (pr);
}

void update_prefixes (prefixes *pr, char next_chr)
{
    assert (pr != NULL && "pointer can't be NULL");

    for (size_t i = 0; i < pr->max_len-1; ++i)
    {
        pr->buf[i] = pr->buf[i+1];
    }

    pr->buf[pr->max_len-1] = next_chr;
}

const char *get_prefix (const prefixes *pr, size_t len)
{
    assert (len <= pr->max_len && "Invalid len");
    assert (pr != NULL && "pointer can't be NULL");
    
    return pr->buf + (pr->max_len-len);
}