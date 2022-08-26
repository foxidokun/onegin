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

    if (file_len_tmp == -1) { return NULL; }

    size_t file_len      = (size_t) file_len_tmp;

    struct text *file    = (struct text *) calloc (1,        sizeof (struct text));
    file->content        = (char *)        calloc (file_len, sizeof (char));
    file->content_size   = file_len;
    char *content_p      = file->content;

    unsigned int n_lines = count_lines (content_p, file_len);
    file->lines          = (struct line *) calloc (n_lines, sizeof (struct line));
    file->cnt            = n_lines;

    fread (content_p, sizeof (char), file_len, stream);
    if (ferror (stream)) return NULL;

    create_index(file);

    return file;
}

int write_lines (const struct text *text, FILE *stream)
{
    assert (text   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    struct line *cur_line = NULL;

    for (unsigned int i = 0; i < text->cnt; ++i)
    {
        cur_line = text->lines + i;

        if (fwrite (cur_line->content, sizeof (char), cur_line->len, stream) != cur_line->len)
        {
            return -1;
        }
    }

    return 0;
}

int write_buf (const struct text *text, FILE *stream)
{
    assert (text   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    size_t n_written = fwrite (text->content, sizeof (char), text->content_size, stream);

    if (n_written == text->content_size) return +0;
    else                                 return -1;
}

ssize_t file_size (FILE *stream)
{
    assert (stream != NULL && "pointer can't be NULL");

    fseek (stream, 0, SEEK_END);
    ssize_t file_len = ftell (stream);
    fseek (stream, 0, SEEK_SET);

    return file_len;
}

unsigned int count_lines (const char *text, size_t text_len)
{
    assert (text != NULL && "pointer can't be NULL");

    unsigned int n_lines = 0;

    for (unsigned int i = 0; i < text_len; ++i)
    {
        if (text[i] == '\n')
        {
            n_lines++;
        }
    }

    return n_lines;
}

void create_index (struct text *text)
{
    size_t file_len = text->content_size;

    unsigned int line_len = 0;
    unsigned int n_line   = 0;
    char *content         = text->content;
    char *line_start      = text->content;

    for (unsigned int i = 0; i < file_len; ++i)
    {
        line_len++;

        if (content[i] == '\n')
        {
            text->lines[n_line].content = line_start;
            text->lines[n_line].len     = line_len;

            n_line++;
            line_start = content + i + 1;
            line_len = 0;
        }
    }
}

void free_text (struct text *text)
{
    assert (text != NULL && "pointer can't be NULL");

    free (text->content);
    free (text->lines);
    free (text);
}