#include <stdio.h>

struct string
{
    char* buff;
    int len;
    int capacity;
};
struct string* new_buff_str();

void append_str(struct string* str, const char* ap);

void append_buff(struct string* str, void* buff, int len);

void free_buff_str(struct string* str);
