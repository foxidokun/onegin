#include <stdio.h>
#include <string.h>
#include "onegin.h"
#include "sort.h"
#include "file.h"

int main (int argc, char *argv[])
{
    if (argc != 3 || (argc == 2 && strcmp (argv[1], "-h") == 0))
    {
        printf ("Onegin sorter\n");
        printf ("Usage: onegin <input file> <output file>\n");
        return 0;
    } 

    FILE *in_stream  = fopen (argv[1], "r");
    FILE *out_stream = fopen (argv[2], "w");

    if (in_stream  == NULL) { printf ("Failed to open input file");  return -1; }
    if (out_stream == NULL) { printf ("Failed to open output file"); return -1; }

    struct file *file = read_file (in_stream);

    if (file == NULL) { printf ("Failed to read lines or OOM\n"); return -1; }

    fprintf (out_stream, "=== Alphabetical order ===\n\n");
    alpha_file_lines_sort (file);
    write_lines (file, out_stream);

    fprintf (out_stream, "\n=== Original order ===\n\n");
    write_buf (file, out_stream);

    fprintf (out_stream, "\n=== Reverse order===\n\n");
    rev_alpha_file_lines_sort (file);
    write_lines (file, out_stream);

    free_file (file);

    fclose ( in_stream);
    fclose (out_stream);
}