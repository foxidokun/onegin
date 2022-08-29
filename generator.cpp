#include <assert.h>
#include <string.h>
#include "onegin.h"
#include "sort.h"
#include "generator.h"

//---------------------------------------------------------------------------------------------------------

/// Hashmap size in struct chain. Choosed for 32MiB hashmap size
static unsigned int HASHMAP_SIZE = 4194304;
/// Number of possible chars in stat::seq and stat::prop
static unsigned int CHAR_CNT     = 256;

//---------------------------------------------------------------------------------------------------------

static int check_bit (uint8_t byte, char index);
static void set_bit (uint8_t *byte, char index, char value);

static long int min (long int a, long int b);
static long int max (long int a, long int b);

static long int find_candidate (const struct text *text, unsigned int from,
                                unsigned int to, uint8_t *used);

//---------------------------------------------------------------------------------------------------------

#define __UNWRAP(expr) { if ((expr) == ERROR) { return ERROR; } }

//---------------------------------------------------------------------------------------------------------

/**
 * @brief Markov chain prefix stats
 */
struct stat
{
    char          *seq;    /// Array of next characters
    unsigned int *prop;    /// Array of char counters 
    unsigned int total;    /// Sum of counters
    char       *prefix;    /// Prefix for this chars
    stat         *next;    /// Ponter to next stat in hashmap
};

struct chain
{
    stat **hashmap;
};

//---------------------------------------------------------------------------------------------------------

chain *create_chain ()
{
    chain *ch    = (chain *) calloc (1, sizeof (struct chain));
    ch->hashmap  = (stat**)  calloc (HASHMAP_SIZE, sizeof (stat *));

    return ch;
}

void free_chain (chain *ch)
{
    assert (ch != NULL && "pointer can't be NULL");

    stat **hashmap = ch->hashmap;
    stat *st            = NULL;
    stat *st_next       = NULL;

    for (unsigned int i = 0; i < HASHMAP_SIZE; ++i)
    {
        st      = hashmap[i];

        while (st != NULL)
        {
            st_next = st->next;

            free (st->seq);
            free (st->prop);
            free (st->prefix);
            free (st);

            st = st_next;
        }
    }

    free (ch->hashmap);
    free (ch);
}

/**
 * @brief      Generates hash in range [0, HASHMAP_SIZE) with djb2 algorithm
 *
 * @param      str   The string
 *
 * @return     Hash
 */
unsigned int hash(const unsigned char *str)
{
    unsigned int hash = 5381;
    int c;

    while ((c = *str++) != '\0')
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % HASHMAP_SIZE;
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
        cand_num_tmp = find_candidate (text, (unsigned int) max (0, ((long int) pos[parity]) - range),
                        (unsigned int) min (n_lines, ((long int) pos[parity]) + range + 1), used);

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

    unsigned int *cand_list = (unsigned int *) calloc (to - from, sizeof (int));    

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

static long int max (long int a, long int b)
{
    return (a > b) ? a : b;
}

static long int min (long int a, long int b)
{
    return (a < b) ? a : b;
}