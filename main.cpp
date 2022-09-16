#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "onegin.h"
#include "sort.h"
#include "file.h"
#include "generator.h"

#ifdef TEST

#include "test.h"

int main ()
{
    run_tests ();
}

#else

int main (int argc, char *argv[])
{
    const int POEM_NLINES       = 256;
    const int AFFINITY_RANGE    =  15;
    const int MARKOV_BUF_SIZE   = 256;
    const int MARKOV_MAX_PREFIX =   5;

    srand ((unsigned int) time (NULL));
    setlocale (LC_CTYPE, "ru_RU.CP1251");

    if (argc != 3 || (argc == 2 && strcmp (argv[1], "-h") == 0))
    {
        printf ("Onegin sorter\n");
        printf ("Usage: onegin <input file> <output file>\n");
        return 0;
    } 

    FILE *in_stream  = fopen (argv[1], "r");
    FILE *out_stream = fopen (argv[2], "w");

    if (in_stream  == NULL) { printf ("Failed to open input file" ); return ERROR; }
    if (out_stream == NULL) { printf ("Failed to open output file"); return ERROR; }

    struct text *file   = read_text (in_stream);
    fclose (in_stream);

    if (file == NULL) { printf ("Failed to read file or OOM\n"); return -1; }

    // ------- DIFFERRENT ORDERS -------

    fprintf (out_stream, "=== Alphabetical order ===\n\n");
    alpha_file_lines_sort (file, cust_qsort);
    write_lines (file, out_stream);

    fprintf (out_stream, "\n=== Original order ===\n\n");
    write_buf (file, out_stream);

    fprintf (out_stream, "\n=== Reverse order===\n\n");
    rev_alpha_file_lines_sort (file, cust_qsort);
    write_lines (file, out_stream);

    // ------- POEM GENERATOR -------

    char **poem = (char **) calloc (POEM_NLINES, sizeof (char*));
    
    if (poem_generator(file, poem, POEM_NLINES, AFFINITY_RANGE) == ERROR)
    {
        fprintf (stderr,     "Failed to generate poem\n");
        fprintf (out_stream, "Failed to generate poem\n");
    }
	else
	{
        fprintf (out_stream, "\n=== Generated poem: ===\n\n");

        for (int i = 0; i < POEM_NLINES; ++i)
        {
            fprintf (out_stream, "%s\n", poem[i]);
        }
	}
    // ------- CHAIN GENERATOR -------

    chain *ch = create_chain (MARKOV_MAX_PREFIX);
    _UNWRAP_NULL_ERR (ch);
    collect_stats (file, ch);

    char *buf = (char *) calloc (MARKOV_BUF_SIZE, sizeof (char));
    _UNWRAP_NULL_ERR (buf);

    fprintf (out_stream, "\n=== MARKOV GENERATED ===\n\n");
    markov_generator(ch, buf, MARKOV_BUF_SIZE);
    fprintf (out_stream, "%s\n", buf);
    
    // ------- CLEANUP -------

    free (poem);
    free (buf);
    free_text (file);
    free_chain (ch);
    fclose (out_stream);

    return 0;
}

#endif //TEST
