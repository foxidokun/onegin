#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "onegin.h"
#include "file.h"


static const unsigned int LINES_CNT = 32;

// Минусы: двойной проход по буферу
struct file_lines *read_lines (FILE *stream)
{
    assert (stream != NULL && "pointer can't be NULL");

    fseek (stream, 0, SEEK_END);
    ssize_t file_len_tmp = ftell (stream);
    fseek (stream, 0, SEEK_SET);

    if (file_len_tmp == -1) { return NULL; }

    size_t file_len = (size_t) file_len_tmp;

    unsigned int n_lines  = 0;

    struct file_lines *lines = (struct file_lines *) calloc (1,        sizeof (struct file_lines));
    lines->content           = (char *)              calloc (file_len, sizeof (char));
    lines->content_size      = file_len;

    char *content_p = lines->content;

    fread (content_p, sizeof (char), file_len, stream);

    for (unsigned int i = 0; i < file_len; ++i)
    {
        if (content_p[i] == '\n')
        {
            n_lines++;
        }
    }

    lines->lines = (struct line *) calloc (n_lines, sizeof (struct line));
    lines->cnt   = n_lines;

    unsigned int line_len = 0;
    unsigned int n_line   = 0;
    char *line_start      = content_p;

    for (unsigned int i = 0; i < file_len; ++i)
    {
        line_len++;

        if (content_p[i] == '\n')
        {
            lines->lines[n_line].content = line_start;
            lines->lines[n_line].len     = line_len;

            n_line++;
            line_start = content_p + i + 1;
            line_len = 0;
        }
    }

    if (ferror (stream)) return  NULL;
    else                 return lines; 
}

int write_lines (const struct file_lines *file, FILE *stream)
{
    assert (file   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    struct line *cur_line = NULL;

    for (unsigned int i = 0; i < file->cnt; ++i)
    {
        cur_line = file->lines + i;

        if (fwrite (cur_line->content, sizeof (char), cur_line->len, stream) != cur_line->len)
        {
            return -1;
        }
    }

    return 0;
}

int write_buf (const struct file_lines *file, FILE *stream)
{
    assert (file   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    size_t n_written = fwrite (file->content, sizeof (char), file->content_size, stream);

    if (n_written == file->content_size) return +0;
    else                                 return -1;
}

void free_file_lines (struct file_lines *file)
{
    free (file->content);
    free (file->lines);
    free (file);
}