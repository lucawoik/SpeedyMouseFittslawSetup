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
    return (long long)time.tv_sec * 1000000l + (long long)time.tv_usec;
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
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

// swap array elements
void swap(Tupel *a, Tupel *b)
{
    Tupel temp = *a;
    *a = *b;
    *b = temp;
}

int calculateChecksum(int array[])
{
    int checksum = 0;

    for (int i = 0; i < NUM_CIRCLES; i++)
    {
        checksum += array[i];
    }

    return checksum;
}

float normalize(float value, char axis)
{
    if (axis=='x')
    {
        return (value - X_MIN) / X_RANGE;
    }
    else if (axis=='y')
    {
        return (value - Y_MIN) / Y_RANGE;
    }
    return -1.0f;
}

float denormalize(float value, char axis)
{
    if (axis=='x')
    {
        return value * X_RANGE + X_MIN;
    }
    else if (axis=='y')
    {
        return value * Y_RANGE + Y_MIN;
    }
    return -1.0f;
}
