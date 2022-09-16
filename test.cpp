#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "onegin.h"
#include "prefixes.h"
#include "hashmap.h"
#include "bits.h"
#include "sort.h"
#include "test.h"

#define R "\033[91m"
#define G "\033[92m"
#define D "\033[39m"

#define _TEST(cond)                                             \
{                                                               \
    if (cond)                                                   \
    {                                                           \
        printf (R "Test FAILED: %s\n\n" D, #cond);              \
        failed++;                                               \
    }                                                           \
    else                                                        \
    {                                                           \
        printf (G "Test OK:     %s\n\n" D, #cond);              \
        success++;                                              \
    }                                                           \
}

#define _ASSERT(cond)                                                           \
{                                                                               \
    if (!(cond))                                                                \
    {                                                                           \
        printf (R "## Test Error: %s##\n" D, __func__);                         \
        printf ("Condition check failed: %s\n", #cond);                         \
        printf ("Test location: File: %s Line: %d\n", __FILE__, __LINE__);      \
        return -1;                                                              \
    }                                                                           \
}

static int intcmp (const void *lhs, const void *rhs);
static int cust_double_cmp (const void *lhs, const void *rhs);

// ------------------------------------------------------------------------------------------------
//                                  SORT TESTS
// ------------------------------------------------------------------------------------------------


int test_cust_qsort ()
{
    const int ARRAY_SIZE = 1024;
    const int NUM_TEST   =  100;
    int *array = (int *) calloc (ARRAY_SIZE, sizeof (int));

    for (int n = 0; n < NUM_TEST; ++n)
    {
        for (int i = 0; i < ARRAY_SIZE; ++i)
        {
            array[i] = rand();
        }

        cust_qsort(array, ARRAY_SIZE, sizeof (int), intcmp);

        for (int i = 1; i < ARRAY_SIZE; ++i)
        {
            if (array[i] < array[i-1])
            {
                free (array);
                _ASSERT (0);
            }
        }
    }

    free (array);

    const int PREMADE_ARRAY_SIZE = 14;
    double premade_array[14] = {14.00, 1.01, 10.02, 11.03, 2.04, 5.05, 8.06, 11.07, 4.08, 7.09, 15.10, 3.11, 13.12, 8.13};
    cust_qsort(premade_array, PREMADE_ARRAY_SIZE, sizeof (double), cust_double_cmp);

    // If value has changed during sorting, this is an error
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"

    _ASSERT (premade_array[0]  == 1.01);
    _ASSERT (premade_array[1]  == 2.04);
    _ASSERT (premade_array[2]  == 3.11);
    _ASSERT (premade_array[3]  == 4.08);
    _ASSERT (premade_array[4]  == 5.05);
    _ASSERT (premade_array[5]  == 7.09);
    _ASSERT (premade_array[6]  == 8.13 || premade_array[6]  == 8.06);
    _ASSERT (premade_array[7]  == 8.13 || premade_array[7]  == 8.06);
    _ASSERT (premade_array[8]  == 10.02);
    _ASSERT (premade_array[9]  == 11.03 || premade_array[9]   == 11.07);
    _ASSERT (premade_array[10] == 11.03 || premade_array[10]  == 11.07);
    _ASSERT (premade_array[11] == 13.12);
    _ASSERT (premade_array[12] == 14.00);
    _ASSERT (premade_array[13] == 15.10);

    #pragma GCC diagnostic pop

    return 0;
}

int test_skip_nalpha_cp1251 ()
{
    char test_str[] = "   ?)!%^*__++!@#$%<?>/-* ,TP";

    _ASSERT (*skip_nalpha_cp1251 (test_str) == 'T');

    return 0;
}

int test_rev_skip_nalpha_cp1251 ()
{
    char test_str[] = "TP    ?)!%^*__++!@#$%<?>/-* ,";
    size_t str_len  = sizeof (test_str) / sizeof (char);

    _ASSERT (rev_skip_nalpha_cp1251 (test_str, str_len - 1) == 1);

    return 0;
}

// ------------------------------------------------------------------------------------------------
//                                      HASHMAP TESTS
// ------------------------------------------------------------------------------------------------

long unsigned int hash (const void *key);

int test_hashmap ()
{
    hashmap *map = hashmap_create (4, sizeof (int), sizeof (int), hash, intcmp);
    _ASSERT (map != NULL);

    const int key = 6, val = 4, w_key = 228;
    _ASSERT (hashmap_insert (map, &key, sizeof(key), &val, sizeof(val)) == 0);
    _ASSERT (hashmap_get (map, &key) != NULL);
    _ASSERT (intcmp (hashmap_get (map, &key), &val) == 0);
    _ASSERT (hashmap_get (map, &w_key) == NULL);

    map = hashmap_resize (map, 12);
    _ASSERT (map != NULL);
    _ASSERT (hashmap_insert (map, &w_key, sizeof(w_key), &val, sizeof(val)) == 0);
    _ASSERT (hashmap_get (map, &key)   != NULL);
    _ASSERT (hashmap_get (map, &w_key) != NULL);
    _ASSERT (intcmp (hashmap_get (map,   &key), &val) == 0);
    _ASSERT (intcmp (hashmap_get (map, &w_key), &val) == 0);


    hashmap_free (map);
    return 0;
}

long unsigned int hash (const void *key)
{
    return *(const unsigned int *) key;
}

// ------------------------------------------------------------------------------------------------
//                                      BITS TESTS
// ------------------------------------------------------------------------------------------------

int test_bits ()
{
    bitflags *bf = create_bitflags (253);
    _ASSERT (bf != NULL);

    set_bit_true (bf, 0);
    _ASSERT (check_bit (bf, 0));
    _ASSERT (bit_find_value (bf, 1, 0) == 0);
    _ASSERT (bit_find_value (bf, 0, 0) == 1);
    _ASSERT (bit_find_value (bf, 1, 1) == 0);
    
    set_bit_false (bf, 0);
    _ASSERT (!check_bit (bf, 0));
    _ASSERT (bit_find_value (bf, 1, 0) == ERROR);
    _ASSERT (bit_find_value (bf, 0, 0) == 0);
    
    set_bit_true (bf, 3);
    _ASSERT (bit_find_value (bf, 1, 0) == 3);
    
    clear_bitflags (bf);
    _ASSERT (bit_find_value (bf, 1, 0) == ERROR);
    _ASSERT (!check_bit (bf, 3));

    set_bit_true (bf, 251);
    _ASSERT (check_bit (bf, 251));

    free_bitflags (bf);
    return 0;
}

// ------------------------------------------------------------------------------------------------
//                                      PREFIXES TESTS
// ------------------------------------------------------------------------------------------------

int test_prefixes ()
{
    const char *str = "DEADINSIDE";

    prefixes *pr = create_prefixes(4, str);
    _ASSERT (pr != NULL);
    _ASSERT (!strcmp (get_prefix(pr, 4), "SIDE"));
    _ASSERT (!strcmp (get_prefix(pr, 3),  "IDE"));
    _ASSERT (!strcmp (get_prefix(pr, 2),   "DE"));
    _ASSERT (!strcmp (get_prefix(pr, 1),    "E"));
    _ASSERT (!strcmp (get_prefix(pr, 0),     ""));

    update_prefixes(pr, 'B');
    update_prefixes(pr, 'T');

    _ASSERT (!strcmp (get_prefix(pr, 4), "DEBT"));
    _ASSERT (!strcmp (get_prefix(pr, 3),  "EBT"));
    _ASSERT (!strcmp (get_prefix(pr, 2),   "BT"));
    _ASSERT (!strcmp (get_prefix(pr, 1),    "T"));
    _ASSERT (!strcmp (get_prefix(pr, 0),     ""));

    free_prefixes(pr);

    return 0;
}

// ------------------------------------------------------------------------------------------------
//                                      GLOBAL FUNCTIONS
// ------------------------------------------------------------------------------------------------

void run_tests ()
{
    unsigned int success = 0;
    unsigned int failed  = 0;

    printf ("Starting tests...\n\n");

    _TEST (test_cust_qsort ()            );
    _TEST (test_skip_nalpha_cp1251 ()    );
    _TEST (test_rev_skip_nalpha_cp1251 ());
    _TEST (test_hashmap ()               );
    _TEST (test_bits ()                  );
    _TEST (test_prefixes ()              );

    printf ("Tests total: %u, failed %u, success: %u, success ratio: %3.1lf%%\n",
        failed + success, failed, success, success * 100.0 / (success + failed));
}

static int intcmp (const void *lhs, const void *rhs)
{
    int lhs_v = * ((const int *) lhs);
    int rhs_v = * ((const int *) rhs);

    if      (lhs_v > rhs_v) return +1;
    else if (lhs_v < rhs_v) return -1;
    else                    return +0;
}

/**
 * @brief      Compare double values by their trunc() components
 *
 * @param[in]  lhs   The left hand side
 * @param[in]  rhs   The right hand side
 *
 * @return     as in strcmp
 */
static int cust_double_cmp (const void *lhs, const void *rhs)
{
    int lhs_v = (int) trunc (* ((const double *) lhs));
    int rhs_v = (int) trunc (* ((const double *) rhs));

    if      (lhs_v > rhs_v) return +1;
    else if (lhs_v < rhs_v) return -1;
    else                    return +0;
}