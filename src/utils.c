#include "inc/utils.h"

void memset(void *ptr, unsigned char value, unsigned int num)
{
    unsigned char *p;
    unsigned int i;

    p = (unsigned char *)ptr;
    i = 0;
    while (i < num)
    {
        p[i] = value;
        i++;
    }
}