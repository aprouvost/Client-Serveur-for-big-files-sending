#include "common.h"


void die(char *s)
{
    perror(s);
    exit(1);
}

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}
