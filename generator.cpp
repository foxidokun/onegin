#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
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

static void free_by_pointer (void *st);

//---------------------------------------------------------------------------------------------------------

int collect_stats (const text *text, chain *ch)
{
    assert (text != NULL && "pointer can't be NULL");
    assert (ch   != NULL && "pointer can't be NULL");

    size_t max_prefix_len = ch->max_prefix_len;

    prefixes *pr = create_prefixes (max_prefix_len, text->content);
    _UNWRAP_NULL_ERR (pr);

    char next_char = '\0';
    stat **st_p    = NULL;
    stat  *st      = NULL;

    // lines[n_line].content always > content
    for (size_t i = max_prefix_len; i < text->content_size; ++i)
    {
        next_char = text->content[i];
        if (next_char == '\0') next_char = ' ';

        for (size_t p_len = 0; p_len <= max_prefix_len; ++p_len)
        {
            st_p = (stat **) hashmap_get (ch->map, get_prefix (pr, p_len));

            if (st_p == NULL)
            {
                st = (stat *) calloc (1, sizeof (stat));
                _UNWRAP_NULL_ERR (st);

                if (hashmap_insert (ch->map, get_prefix (pr, p_len), p_len+1, &st, sizeof (st)) == ERROR)
                {
                    hashmap *new_map = hashmap_resize (ch->map, ch->map->allocated * 2);
                    _UNWRAP_NULL_ERR (new_map);
                    ch->map = new_map;
                }
            }
            else
            {
                st = *st_p;
            }

            st->total++;
            st->char_cnt[(unsigned char) next_char]++;
        }

        update_prefixes (pr, text->content[i]);
    }

    free_prefixes (pr);

    return 0;
}

int markov_generator (const chain *ch, char *buf, size_t buf_size)
{
    assert (ch   != NULL && "pointer can't be NULL");
    assert (buf  != NULL && "pointer can't be NULL");
    assert (ch->max_prefix_len <= buf_size && "Small buffer");

    size_t pos       =   0 ;
    char   next_char = '\0';

    // Fill buf with '\0'
    memset (buf, '\0', buf_size);

    // Do not overwrite last '\0'
    while (buf_size > 1)
    {
        while ((next_char = get_next_char (ch, buf)) == '\0')
        {
            // Failed to insert even with empty prefix
            if (pos == 0) return ERROR;

            buf++;
            pos--;
        }

        buf[pos] = next_char;
        pos++;
        buf_size--;
    }

    return 0;
}

char get_next_char (const chain *ch, const char *prefix)
{
    assert (ch     != NULL && "pointer can't be NULL");
    assert (prefix != NULL && "pointer can't be NULL");

    stat **st_p = (stat **) hashmap_get(ch->map, prefix);
    if (st_p == NULL) return '\0';

    const stat *const st = *st_p;

    assert (st->total != 0 && "Empty stat in hashmap");

    // rand generates in [0, RAND_MAX] so it's already unsigned
    unsigned long int target_cnt = (unsigned long int) rand() % st->total;
    unsigned long int   real_cnt = 0;
    unsigned      int        chr = 0;

    for (chr = 0; real_cnt <= target_cnt; ++chr)
    {
        real_cnt += st->char_cnt[chr];

        assert (chr < 256 && "total_cnt > sum of all char_cnt");
    }

    assert ((chr-1) < 256 && "Ooops... Bad type casting");
    assert (chr     >   0 && "Ooops... Bad type casting");

    return (char) (chr-1);
}

int poem_generator (const struct text *text, char **buf, unsigned int buf_size,
                        unsigned char range)
{
    assert (text != NULL  && "pointer can't be NULL");
    assert (buf  != NULL  && "pointer can't be NULL");
    assert (buf_size >= 2 && "can't construct poem from less than two lines");

    line *lines          = text->lines;
    unsigned int n_lines = text->n_lines;

    bitflags *used       = create_bitflags (n_lines);

    unsigned int pos[2]  = {};

    long     int  pos_tmp      = 0;
    unsigned int  cand_num     = 0;
    long     int  cand_num_tmp = 0;
    unsigned char parity       = 0;

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
            free_bitflags (used);
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
                if (isalpha(lines[n].content[i])) alpha_cnt++;
            }

            if (((long double) alpha_cnt) / line_len < MIN_ALPHA_PERSENTAGE) continue;

            cand_list[cand_num] = n;
            cand_num++;
        }
    }

    if (cand_num == 0)
    {
        free (cand_list);
        return ERROR;
    }

    unsigned int cand = cand_list[(unsigned int) rand() % cand_num];

    set_bit_true (used, cand);

    free (cand_list);
    
    return cand;
}

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

    hashmap_forall (ch->map, free_by_pointer); // Free all allocated stat structers
    hashmap_free   (ch->map);
    free (ch);
}

/**
 * @brief      Free allocated memory by (void **)
 *
 * @param      pp    void **
 */
static void free_by_pointer (void *pp)
{
    assert (pp != NULL && "pointer can't be NULL");

    free (* (void**)pp);
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