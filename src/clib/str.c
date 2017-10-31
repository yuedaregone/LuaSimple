#include "str.h"
#include <stdlib.h>
#include <string.h>

struct string* new_buff_str()
{
    struct string* bf = (struct string*)malloc(sizeof(struct string));
    bf->buff = NULL;
    bf->len = 0;
    bf->capacity = 0;
    return bf;
}

void append_str(struct string* str, const char* ap)
{
    int len = strlen(ap);
    if (str->buff == NULL)
    {
        str->capacity = len + 1;
        str->buff = (char*)malloc(str->capacity);
        strncpy(str->buff, ap, len);
        str->buff[len] = '\0';
        str->len = len;
    }
    else
    {
        if (str->capacity - str->len >= len + 1)
        {
            strncpy(str->buff + str->len, ap, len);
            str->len += len;
        }
        else
        {
            int size = str->capacity > len ? 2*str->capacity : 2*len;
            str->buff = (char*)realloc(str->buff, size);
            strncpy(str->buff + str->len, ap, len);
            str->capacity = size;
            str->len += len;
        }
    }
}

void append_buff(struct string* str, void* buff, int len)
{
    if (str->buff == NULL)
    {
        str->capacity = len + 1;
        str->buff = (char*)malloc(str->capacity);
        strncpy(str->buff, buff, len);
        str->buff[len] = '\0';
        str->len = len;
    }
    else
    {
        if (str->capacity - str->len >= len + 1)
        {
            strncpy(str->buff + str->len, buff, len);
            str->len += len;
        }
        else
        {
            int size = str->capacity > len ? 2*str->capacity : 2*len;
            str->buff = (char*)realloc(str->buff, size);
            strncpy(str->buff + str->len, buff, len);
            str->capacity = size;
            str->len += len;
        }
    }
}

void free_buff_str(struct string* buff)
{
    free(buff->buff);
    free(buff);
}
