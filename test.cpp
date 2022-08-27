#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "test.h"

#define _TEST(cond)                                 \
{                                                   \
    if (cond)                                       \
    {                                               \
        printf ("Test FAILED: %s\n\n", __func__);   \
        failed++;                                   \
    }                                               \
    else                                            \
    {                                               \
        printf ("Test OK:     %s\n\n", __func__);   \
        success++;                                  \
    }                                               \
}

static int intcmp (const void *lhs, const void *rhs);

int test_cust_qsort ()
{
    const int ARRAY_SIZE = 1024;
    int *array = (int *) calloc (1024, sizeof (int));

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

    free (array);
    return 0;
}

void run_tests ()
{
    unsigned int success = 0;
    unsigned int failed  = 0;

    printf ("Starting tests...\n\n");

    _TEST (test_cust_qsort());

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