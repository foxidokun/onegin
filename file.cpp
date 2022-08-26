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

    size_t  buf_size  = 64;
    ssize_t line_size =  0;
    unsigned int n = 0;
    struct file_lines *lines = create_file_lines (LINES_CNT);
    char *buf = (char *) calloc (buf_size, sizeof (char));
    char *content = NULL;

    while ((line_size = getline(&buf, &buf_size, stream)) != -1)
    {
        assert (line_size >= 0 && "Wrong number of written characters");

        n++;

        // Allocate buffer for line content
        content = (char *) calloc ((size_t) line_size, sizeof (char));

        if (content == NULL) return NULL;

        // Copy all content from buf except '\n',
        // instead '\0' will be inserted by strncpy
        strncpy (content, buf, (size_t) line_size - 1);

        // Insert line
        insert_line(&lines, content, n);
    }

    free (buf);

    if (feof (stream)) return lines;
    else               return  NULL; 
}
 
int insert_line (struct file_lines **lines, char *content, unsigned int number)
{
    assert (lines   != NULL && "pointer can't be NULL");
    assert (*lines  != NULL && "pointer can't be NULL");
    assert (content != NULL && "pointer can't be NULL");

    struct file_lines *lines_v = *lines;

    if (lines_v->cnt == lines_v->alloc_cnt)
    {
        unsigned int alloc_cnt = lines_v->alloc_cnt;
        struct line *lines_tmp = lines_v->lines;

        lines_tmp = (struct line *) realloc(lines_v->lines, alloc_cnt * 2 * sizeof (struct line));

        if (lines_tmp == NULL) return -1;

        lines_v->lines      = lines_tmp;
        lines_v->alloc_cnt *= 2;
    }

    lines_v->lines[lines_v->cnt].content = content;
    lines_v->lines[lines_v->cnt].number  = number;
    lines_v->cnt++;

    *lines = lines_v;

    return 0;
}

int write_lines (const struct file_lines *file, FILE *stream)
{
    assert (file   != NULL && "pointer can't be NULL");
    assert (stream != NULL && "pointer can't be NULL");

    for (unsigned int i = 0; i < file->cnt; ++i)
    {
        if (fputs(file->lines[i].content, stream) == EOF)
        {
            return -1;
        }

        fputc ('\n', stream);
    }

    return 0;
}

void free_file_lines (struct file_lines *file)
{
    assert (file != NULL && "pointer can't be NULL");

    for (unsigned int i = 0; i < file->cnt; ++i)
    {
        free (file->lines[i].content);
    }

    free (file->lines);
    free (file);
}

struct file_lines *create_file_lines (unsigned int n)
{
    struct file_lines *f_lines = (struct file_lines *) calloc (1, sizeof (struct file_lines));

    if (f_lines == NULL) return NULL;

    f_lines->alloc_cnt = n;
    f_lines->cnt = 0;
    f_lines->lines = (struct line*) calloc (n, sizeof (struct line));

    if (f_lines->lines == NULL) return NULL;

    return f_lines;
}