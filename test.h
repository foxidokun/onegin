/**
 * @brief      Run all test
 */
void run_tests ();

/**
 * @note Requires initialized rand
 *
 * @return     Non-zero value on error
 */
int test_cust_qsort ();

/// @return Non-zero value on error
int test_skip_nalpha_cp1251 ();

/// @return Non-zero value on error
int test_rev_skip_nalpha_cp1251 ();

/// @return Non-zero value on error
int test_hashmap ();