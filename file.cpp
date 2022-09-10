#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "onegin.h"
#include "file.h"

struct text *read_text (FILE *stream)
{
    assert (stream != NULL && "pointer can't be NULL");

    ssize_t file_len_tmp = file_size (stream);
    if (file_len_tmp == ERROR) { return NULL; }

    size_t file_len      = (size_t) file_len_tmp;

    struct text *file    = (struct text *) calloc (1,            sizeof (struct text));
    file->content        = (char *)        calloc (file_len + 1, sizeof (char));
    file->content_size   = file_len + 1;
    char *content_p      = file->content;

    if (fread (content_p, sizeof (char), file_len, stream) != file_len || ferror (stream))
    {
        return NULL;
    }

    content_p[file_len]  = '\0';

    unsigned int n_lines = count_lines (content_p);
    file->lines          = (struct line *) calloc (n_lines, sizeof (struct line));
    file->n_lines        = n_lines;

    create_index (file);

    return file;
}

int write_lines (const struct text *text, FILE *stream)
{
    assert (text   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    const struct line *cur_line = NULL;

    for (unsigned int i = 0; i < text->n_lines; ++i)
    {
        cur_line = text->lines + i;

        fputs (cur_line->content, stream);
        putc ('\n', stream);
    }

    return 0;
}

int write_buf (const struct text *text, FILE *stream)
{
    assert (text   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    char  *content       = text->content;
    size_t content_size  = text->content_size;
    unsigned int n_lines = text->n_lines;

    char *out_buf = (char *) calloc (content_size, sizeof (char));
    char *buf_cur = out_buf;

    memcpy (out_buf, content, content_size);

    for (unsigned int i = 0; i < n_lines; ++i)
    {
        buf_cur = strchr (buf_cur, '\0');
        buf_cur[0] = '\n';
        buf_cur++;
    }

    if (fwrite (out_buf, sizeof (char), content_size, stream) != content_size)
    {
        return ERROR;
    }

    free (out_buf);
    return 0;
}

ssize_t file_size (FILE *stream)
{
    assert (stream != NULL && "pointer can't be NULL");

    ssize_t init_pos = ftell (stream);
    if (init_pos != 0) { return ERROR; }

    fseek (stream, 0, SEEK_END);
    ssize_t file_len = ftell (stream);
    fseek (stream, init_pos, SEEK_SET);

    return file_len;
}

unsigned int count_lines (const char *text)
{
    assert (text != NULL && "pointer can't be NULL");

    unsigned int n_lines = 0;

    while ((text = strchr (text, '\n')) != NULL)
    {
        n_lines++;
        text++;
    }

    return n_lines;
}

void create_index (struct text *text)
{
    assert (text != NULL && "pointer can't be NULL");

    unsigned int n_line   = 0;
    char *line_start      = text->content;
    line *lines           = text->lines;
    char *cur             = line_start;

    while ((cur = strchr (line_start, '\n')) != NULL)
    {
        cur[0] = '\0';
        cur++;  // Point to first character of next line

        assert (lines != NULL && "pointer can't be NULL");

        lines[n_line].content = line_start;
        lines[n_line].len     = (size_t) (cur - line_start);

        n_line++;
        line_start = cur;
    }
}

void free_text (struct text *text)
{
    assert (text != NULL && "pointer can't be NULL");

    free (text->content);
    free (text->lines);
    free (text);
}
