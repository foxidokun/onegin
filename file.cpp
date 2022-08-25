#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "onegin.h"
#include "file.h"


static const unsigned int LINES_CNT = 32;

struct file_lines *read_lines (FILE *stream)
{
    assert (stream != NULL && "pointer can't be NULL");

    size_t  buf_size = 64;
    ssize_t line_size = 0;
    unsigned int n = 0;
    struct file_lines *lines = create_file_lines (LINES_CNT);
    char *buf = (char *) calloc (buf_size, sizeof (char));
    char *content = NULL;

    while ((line_size = getline(&buf, &buf_size, stream)) != -1)
    {
        assert (line_size >= 0 && "Wrong number of written characters");

        n++;

        // Allocate buffer for line content
        content = (char *) calloc (line_size, sizeof (char));

        if (content == NULL) return NULL;

        // Copy all content from buf except '\n',
        // instead '\0' will be inserted by strncpy
        strncpy (content, buf, line_size - 1);

        // Insert line
        insert_line(lines, content, n);
    }

    free (buf);

    if (!feof (stream)) return lines;
    else                return  NULL; 
}

int insert_line (struct file_lines **lines, char *content, int number)
{
    assert (lines   != NULL && "pointer can't be NULL");
    assert (*lines  != NULL && "pointer can't be NULL");
    assert (content != NULL && "pointer can't be NULL");

    struct file_lines *lines_v = *lines;

    if (lines_v->alloc_cnt == lines_v->cnt)
    {
        lines_v = (struct file_lines *) realloc(lines_v, lines_v->alloc_cnt * 2);

        if (lines_v == NULL) return -1;

        lines_v->alloc_cnt *= 2;
    }

    lines_v->lines[lines_v->cnt].content = content;
    lines_v->lines[lines_v->cnt].number  = number;
    lines_v->cnt++;

    *lines = lines_v;

    return 0;
}

void free_file_lines (struct file_lines *ptr)
{
    assert (ptr != NULL && "pointer can't be NULL");

    for (int i = 0; i < ptr->cnt; ++i)
    {
        free (ptr->lines[i].content);
    }

    free (ptr);
}

int write_lines (const struct file_lines *lines, FILE *stream)
{
    assert (lines  != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    for (int i = 0; i < lines->cnt; ++i)
    {
        if (fputs(lines->lines[i].content, stream) == EOF)
        {
            return -1;
        }
    }

    return 0;
}