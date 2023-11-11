#include "main.h"

long millis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

long long micros()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (long long) time.tv_sec * 1000000l + (long long) time.tv_usec;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int checkCollision(int x, int y, Target *t)
{
    return (pow(x - t->x, 2) + pow(y - t->y, 2) <= pow(t->r, 2));
}

int calculateDistance(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2 , 2));
}

// swap array elements
void swap(Tupel *a, Tupel *b)
{
	Tupel temp = *a;
    *a = *b;
    *b = temp;
}
