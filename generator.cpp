#include <assert.h>
#include <string.h>
#include <math.h>
#include "onegin.h"
#include "sort.h"
#include "generator.h"
#include "bits.h"
#include "prefixes.h"

//---------------------------------------------------------------------------------------------------------

static long int find_candidate (const struct text *text, unsigned int from,
                                unsigned int to, bitflags *used);

static long int min (long int a, long int b);
static long int max (long int a, long int b);

static long unsigned int strhash (const void *str);
static               int keycmp  (const void *lhs, const void *rhs);

//---------------------------------------------------------------------------------------------------------

chain *create_chain (size_t max_prefix_len)
{
    chain *ch = (chain *) calloc (1, sizeof (struct chain));
    _UNWRAP_NULL (ch);

    ch->map   = hashmap_create (NPREFIXES, (max_prefix_len + 1)*sizeof (char),
                                sizeof (stat *), strhash, keycmp);
    _UNWRAP_NULL (ch->map);

    ch->max_prefix_len = max_prefix_len;

    return ch;
}

void free_chain (chain *ch)
{
    assert (ch != NULL && "pointer can't be NULL");

    hashmap_forall (ch->map, free); // Free all allocated stat structers
    hashmap_free   (ch->map);
    free (ch);
}

int collect_stats (const text *text, chain *ch)
{
    assert (text != NULL && "pointer can't be NULL");
    assert (ch   != NULL && "pointer can't be NULL");

    char  *content        = text->content;
    line  *lines          = text->lines;
    size_t max_prefix_len = ch->max_prefix_len;

    unsigned int n_line = 0;
    while (n_line < text->n_lines && strlen (lines[n_line].content) < max_prefix_len)
    {
        n_line++;
    }
    if (n_line == text->n_lines) return ERROR;

    prefixes *pr = create_prefixes (max_prefix_len, lines[n_line].content);
    _UNWRAP_NULL_ERR (pr);

    char next_char = '\0';
    stat *st = NULL;

    // lines[n_line].content always > content
    for (size_t i = (size_t) (lines[n_line].content - content) + max_prefix_len;
                i < text->content_size; ++i)
    {
        next_char = text->content[i];
        if (next_char == '\0') next_char = ' ';

        for (size_t p_len = 0; p_len <= max_prefix_len; ++p_len)
        {
            st = (stat *) hashmap_get (ch->map, get_prefix (pr, p_len));

            if (st == NULL)
            {
                st = (stat *) calloc (1, sizeof (stat));
                _UNWRAP_NULL_ERR (st);

                if (hashmap_insert (ch->map, get_prefix (pr, p_len), st) == ERROR)
                {
                    hashmap *new_map = hashmap_resize(ch->map, ch->map->allocated * 2);
                    _UNWRAP_NULL_ERR (new_map);
                    ch->map = new_map;
                }
            }

            st->total++;
            st->char_cnt[(unsigned char) next_char]++;
        }

        update_prefixes (pr, text->content[i]);
    }

    return 0;
}

int poem_generator (const struct text *text, char **buf, unsigned int buf_size,
                        unsigned char range)
{
    assert (text != NULL     && "pointer can't be NULL");
    assert (buf  != NULL     && "pointer can't be NULL");
    assert (buf_size >= 2    && "can't construct poem from less than two lines");

    line *lines          = text->lines;
    unsigned int n_lines = text->n_lines;

    bitflags *used       = create_bitflags (n_lines);

    unsigned int pos[2]  = {};

    long int pos_tmp = 0;

    unsigned int cand_num    = 0;
    long int cand_num_tmp    = 0;
    unsigned char parity     = 0;


    for (size_t n = 0; n < buf_size; ++n)
    {
        if (n % BLOCK_SIZE == 0)
        {
            _UNWRAP_ERR (pos_tmp = find_candidate (text, 0, n_lines, used));
            pos[0] = (unsigned int) pos_tmp;
            _UNWRAP_ERR (pos_tmp = find_candidate (text, 0, n_lines, used));
            pos[1] = (unsigned int) pos_tmp;
        }

        parity = n % 2;

        // Find not used variables near pos[parity]
        cand_num_tmp = find_candidate (text, (unsigned int) max (0, ((long int) pos[parity]) - range),
                        (unsigned int) min (n_lines, ((long int) pos[parity]) + range + 1), used);

        if (cand_num_tmp == ERROR)
        {   
            return ERROR;
        }

        cand_num = (unsigned int) cand_num_tmp;

        // Select random candidate
        pos[parity] = cand_num;
        buf[n]      = lines[cand_num].content;
    }

    free_bitflags (used);

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
                                unsigned int to, bitflags *used)
{
    assert (text != NULL       && "pointer can't be NULL");
    assert (from < to          && "range can't be empty");

    unsigned int *cand_list = (unsigned int *) calloc (to - from, sizeof (int));    

    line *lines           = text->lines;
    unsigned int cand_num = 0;
    size_t       line_len = 0;
    size_t      alpha_cnt = 0;

    for (unsigned int n = from; n < to; ++n)
    {
        if (!check_bit (used, n))
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

    set_bit_true (used, cand);

    free (cand_list);
    
    return cand;
}

static long int max (long int a, long int b)
{
    return (a > b) ? a : b;
}

static long int min (long int a, long int b)
{
    return (a < b) ? a : b;
}

static long unsigned int strhash (const void *str)
{
    const unsigned char *str_c = (const unsigned char *) str;

    unsigned long hash = 5381;
    unsigned int c;

    while ((c = *str_c++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static int keycmp  (const void *lhs, const void *rhs)
{
    return strcmp ((const char *) lhs, (const char *) rhs);
}