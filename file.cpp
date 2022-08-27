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

    struct text *file    = (struct text *) calloc (1,            sizeof (struct text));
    file->content        = (char *)        calloc (file_len + 1, sizeof (char));
    file->content_size   = file_len;
    char *content_p      = file->content;

    fread (content_p, sizeof (char), file_len, stream);
    if (ferror (stream)) return NULL;
    content_p[file_len] = '\0';

    unsigned int n_lines = count_lines (content_p);
    file->lines          = (struct line *) calloc (n_lines, sizeof (struct line));
    file->cnt            = n_lines;

    create_index (file);

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

        fputs (cur_line->content, stream);
        putc ('\n', stream);
    }

    return 0;
}

int write_buf (const struct text *text, FILE *stream)
{
    assert (text   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    unsigned int n_lines = text->cnt;
    char *content = text->content;

    for (unsigned int i = 0; i < n_lines; ++i)
    {
        if (fputs (content, stream) == EOF)
        {
            return -1;
        }
        putc ('\n', stream);
        
        content = strchr (content, '\0') + 1;
    }

    return 0;
}

ssize_t file_size (FILE *stream)
{
    assert (stream != NULL && "pointer can't be NULL");

    fseek (stream, 0, SEEK_END);
    ssize_t file_len = ftell (stream);
    fseek (stream, 0, SEEK_SET);

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
            content[i] = '\0';

            assert (text->lines != NULL && "pointer can't be NULL");

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
