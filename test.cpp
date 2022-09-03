#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
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
                return -1;
            }
        }
    }

    free (array);
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
    _ASSERT (hashmap_insert (map, &key, &val) == 0);
    _ASSERT (hashmap_get (map, &key) != NULL);
    _ASSERT (intcmp (hashmap_get (map, &key), &val) == 0);
    _ASSERT (hashmap_get (map, &w_key) == NULL);

    map = hashmap_resize (map, 12);
    _ASSERT (map != NULL);
    _ASSERT (hashmap_insert (map, &w_key, &val) == 0);
    _ASSERT (hashmap_get (map, &key)   != NULL);
    _ASSERT (hashmap_get (map, &w_key) != NULL);
    _ASSERT (intcmp (hashmap_get (map,   &key), &val) == 0);
    _ASSERT (intcmp (hashmap_get (map, &w_key), &val) == 0);

    return 0;
}

long unsigned int hash (const void *key)
{
    return *(const unsigned int *) key;
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

    printf ("Tests total: %u, failed %u, success: %u, success ratio: %3.1lf",
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