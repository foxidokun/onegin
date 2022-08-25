#include "onegin.h"
#include <stdio.h>
#include "sort.h"
#include "file.h"

int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf ("Onegin sorter\n");
        printf ("Usage: onegin <input file> <output file>\n");
        return 0;
    } 

    FILE *in_stream  = fopen (argv[1], "r");
    FILE *out_stream = fopen (argv[2], "w");

    if (in_stream  == NULL) { printf ("Failed to open input file");  return -1; }
    if (out_stream == NULL) { printf ("Failed to open output file"); return -1; }

    struct file_lines *lines = read_lines(in_stream);

    if (lines == NULL) { printf ("Failed to read lines or OOM"); return -1; }

    fprintf (out_stream, "=== Alphabetical order ===\n\n");
    alpha_file_lines_sort (lines);
    write_lines(lines, out_stream);

    fprintf (out_stream, "\n=== Original order ===\n\n");

    num_file_lines_sort(lines);
    write_lines(lines, out_stream);

    fprintf (out_stream, "\n=== Reverse order===\n\n");

    rev_alpha_file_lines_sort (lines);
    write_lines(lines, out_stream);
}