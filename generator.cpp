#include <assert.h>
#include <string.h>
#include <math.h>
#include "onegin.h"
#include "sort.h"
#include "generator.h"

// Для отладки глубины hashmap
#include <stdio.h>

//---------------------------------------------------------------------------------------------------------

static int check_bit (uint8_t byte,  char index);
static void  set_bit (uint8_t *byte, char index, char value);

static long int min (long int a, long int b);
static long int max (long int a, long int b);

static long int find_candidate (const struct text *text, unsigned int from,
                                unsigned int to, uint8_t *used);

static char **allocate_prefixes (unsigned int max_len);
static void shift_prefixes (char **prefixes, unsigned int max_len);
static void initialize_prefixes (char **prefixes, const char *str, unsigned int max_len);
static void update_stats (chain *chain, const char *const *prefixes, unsigned int max_len, unsigned char ch);
static stat *initialize_stat (const char *prefix, unsigned int prefix_len);
static char get_char (const chain *chain, const char *prefix, unsigned int prefix_size);

static unsigned int gen_rand (unsigned int max);
unsigned int hash(const char *str);

//---------------------------------------------------------------------------------------------------------

#define __UNWRAP(expr) { if ((expr) == ERROR) { return ERROR; } }

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

            free (st->prefix);
            free (st);

            st = st_next;
        }
    }

    free (ch->hashmap);
    free (ch);
}

void collect_stats (const text *text, chain *chain)
{
    assert (text != NULL && "pointer can't be NULL");

    char  *content      = text->content;
    size_t content_size = text->content_size;
    char   ch           = '\0';

    char **prefixes = allocate_prefixes (MAX_PREFIX_LEN);

    initialize_prefixes (prefixes, content, MAX_PREFIX_LEN);

    for (size_t pos = MAX_PREFIX_LEN; pos < content_size; ++pos)
    {
        ch = content[pos];
        if (ch == '\0') ch = ' ';

        if (!(ch == ' ' || cp1251_isalpha (ch))) continue;

        update_stats (chain, prefixes, MAX_PREFIX_LEN, (unsigned char) ch);

        shift_prefixes (prefixes, MAX_PREFIX_LEN);

        for (unsigned int i = 0; i < MAX_PREFIX_LEN; ++i)
        {
            prefixes[i][MAX_PREFIX_LEN-1-i] = ch;
        }
    }

    free (prefixes);
}

/**
 * @brief      Update markov chain stats
 *
 * @param      chain           Chain
 * @param[in]  prefixes        Prefixes
 * @param[in]  max_len         The maximum prefix length
 * @param[in]  ch              char
 */
static void update_stats (chain *chain, const char *const *prefixes, unsigned int max_len, unsigned char ch)
{
    assert (chain    != NULL && "pointer can't be NULL");
    assert (prefixes != NULL && "pointer can't be NULL");

    stat *st      = NULL;
    stat *st_prev = NULL;

    int __DEBUG_CNT = 0;

    for (unsigned int i = 0; i <= max_len; ++i)
    {
        st = chain->hashmap[hash (prefixes[i])];

        st_prev = NULL;

        __DEBUG_CNT = 0;

        while (st != NULL && strncmp (st->prefix, prefixes[i], max_len-i) != 0)
        {
            st_prev = st;
            st = st->next;

            __DEBUG_CNT++;
        }

        printf ("Depth: %d\n", __DEBUG_CNT);
        __DEBUG_CNT = 0;

        if (st == NULL)
        {
            st = initialize_stat (prefixes[i], max_len-i);

            if (st_prev != NULL)
            {
                st_prev->next = st;
            }
            else 
            {
                chain->hashmap[hash (prefixes[i])] = st;
            }
        }
        
        st->prop[ch]++;
        st->total++;

    }
}

void generate_text (const chain *chain, char *buf, size_t buf_size, const char *seed)
{
    assert (buf  != NULL && "pointer can't be NULL");
    assert (seed != NULL && "pointer can't be NULL");

    size_t len = strlen (seed);

    assert (len >= MAX_PREFIX_LEN && "seed too small");
    assert (buf_size >= len && "buf_size too small even for seed");

    char **prefixes = allocate_prefixes (MAX_PREFIX_LEN);
    initialize_prefixes(prefixes, seed, MAX_PREFIX_LEN);

    strncpy (buf, seed, len);

    size_t pos = len;
    char   ch  = '\0';

    for (; pos < buf_size; ++pos)
    {
        for (long int p_len = MAX_PREFIX_LEN; p_len >= 0; --p_len)
        {
            ch = get_char (chain, prefixes[MAX_PREFIX_LEN-p_len], (unsigned int) p_len);
            if (ch != '\0') break;
        }

        if (ch == '\0') { assert (0 && "Stats not initialized"); }

        buf[pos] = ch;
    }

    free (prefixes);
}

/**
 * @brief      Generate next character
 *
 * @param      chain        The chain
 * @param[in]  prefix       The prefix
 * @param[in]  prefix_size  The prefix size (>0)
 *
 * @return     Character or '\0' if no character can be generated
 */
static char get_char (const chain *chain, const char *prefix, unsigned int prefix_size)
{
    assert (chain  != NULL && "pointer can't be NULL");
    assert (prefix != NULL && "pointer can't be NULL");

    stat **map = chain->hashmap;
    stat  *st  = map[hash (prefix)];

    while (st != NULL && st->prefix_len != prefix_size &&
          strncmp (st->prefix, prefix, prefix_size) != 0)
    {
        st = st->next;
    }

    if (st == NULL || st->total == 0) 
    {
        return '\0';
    }

    unsigned int rand = gen_rand (st->total);
    unsigned int sum  = 0;
    unsigned char ch  = 0;

    for (ch = 0; sum < rand; ++ch)
    {
        sum += st->prop[ch];
    }

    return (char) ch;
}

/**
 * @brief      Allocates stat and initializes it with zero values
 *
 * @return     Pointer to allcoated stat
 */
static stat *initialize_stat (const char *prefix, unsigned int prefix_len)
{
    assert (prefix != NULL && "pointer can't be NULL");

    stat *st   = (stat *) calloc (1, sizeof (stat));

    st->prefix     = (char *) calloc (prefix_len, sizeof (char));
    st->total      = 0;
    st->next       = NULL;
    st->prefix_len = prefix_len;

    memcpy (st->prefix, prefix, prefix_len);

    return st;
}

/**
 * @brief      Initializes the prefixes with given str 
 *
 * @param      prefixes  Prefixes
 * @param[in]  str       String
 * @param[in]  max_len   The maximum prefix length
 */
static void initialize_prefixes (char **prefixes, const char *str, unsigned int max_len)
{
    assert (str      != NULL && "pointer can't be NULL");
    assert (prefixes != NULL && "pointer can't be NULL");

    for (unsigned int n = 0; n < max_len; ++n)
    {
        memcpy (prefixes[n], str, max_len-n);
    }
}

/**
 * @brief      Shifts prefixes
 *
 * prefixes[i][0] = prefixes[i][1] and so on
 *
 * @param      prefixes  Prefixes
 * @param      max_len   Max prefix len
 */
static void shift_prefixes (char **prefixes, unsigned int max_len)
{
    assert (prefixes != NULL && "pointer can't be NULL");

    for (unsigned int n = 0; n < max_len; ++n)
    {
        assert (prefixes[n] != NULL && "pointer can't be NULL");

        for (unsigned int pos = 0; pos < max_len - 1; ++pos)
        {
            prefixes[n][pos] = prefixes[n][pos+1];
        }
    }
}

/**
 * @brief      Allocates 2D array of prefixes in single chunk (2 callocs)
 *
 * @param[in]  max_len  
 *
 * @return     Pointer to 2D array, where len (return[0]) == max_len, len (return[i]) == len (return[i-1])
 */
static char **allocate_prefixes (unsigned int max_len)
{
    unsigned int array_size = max_len * (max_len + 1) / 2;

    char **prefixes = (char **) calloc (max_len+1,  sizeof (char *));
    prefixes[0]     = (char *)  calloc (array_size, sizeof (char));

    for (unsigned int i = 1; i <= max_len; ++i)
    {
        prefixes[i] = prefixes[i-1] + (max_len - i);
    }

    return prefixes;
}

/**
 * @brief      Generates hash in range [0, HASHMAP_SIZE) with modified djb2 algorithm
 *
 * @param      str   The string
 *
 * @return     Hash
 */
unsigned int hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++) != '\0')
        hash = ((hash << 5) + hash) + (unsigned int) c; /* hash * 33 + c */

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

/**
 * @brief      Generate random unsgined int value in [0, max) range 
 *
 * @note Requires initialised rand
 * 
 * @param[in]  max
 *
 * @return     Rand value
 */
static unsigned int gen_rand (unsigned int max)
{
    return (unsigned int) round (rand () * ((double) max) / RAND_MAX);
}