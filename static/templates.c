#include "main.h"

/* neue belegung: x,y(die des ziels), radius, distance to the center */
/* TODO: macht das Sinn aus einem Target ein anderes zu bilden? */
Target createTarget(Target t)
{
    Target target;

    target.r = t.r;
    target.y = t.y;
    target.x = t.x;
    target.d = t.d;

    return target;
}

void createTargetArray()
{
    int NUM_TUPELS = NUM_DISTANCE * NUM_RADIUS;
    Tupel TO_BE_RANDOMIZED[NUM_TUPELS];
    for (int i = 0; i < NUM_DISTANCE; i++)
    {
        for (int j = 0; j < NUM_RADIUS; j++)
        {
            TO_BE_RANDOMIZED[NUM_RADIUS * i + j].radius = TARGET_RADIUS[j];
            TO_BE_RANDOMIZED[NUM_RADIUS * i + j].distance = TARGET_DISTANCE[i];
            // printf("radius: %d, durchmesser: %d\n", TO_BE_RANDOMIZED[NUM_RADIUS * i + j].radius, TO_BE_RANDOMIZED[NUM_RADIUS * i + j].distance);
        }
        // printf("\n");
    }
    /* printf("------- \n"); */
    for (int m = 0; m < NUM_TUPELS; m++)
    {
        Tupel *a = &TO_BE_RANDOMIZED[m];
        Tupel *b = &TO_BE_RANDOMIZED[rand() % (NUM_TUPELS)];
        swap(a, b);
    }

    /* for debug */
    /* for (int i = 0; i < NUM_DISTANCE; i++)
    {
        for (int j = 0; j < NUM_RADIUS; j++)
        {
            printf("radius: %d, durchmesser: %d\n", TO_BE_RANDOMIZED[NUM_RADIUS * i + j].radius, TO_BE_RANDOMIZED[NUM_RADIUS * i + j].distance);
        }
        printf("\n");
    } */

    float angle, step;
    step = (2 * M_PI) / NUM_CIRCLES;
    int stepNum = floor(NUM_CIRCLES / 2);

    for (int k = 0; k < NUM_TUPELS; k++)
    {
        int currentTarget = 0;
        /*  printf(" erster currentTarget: %d \n", currentTarget); */
        for (int l = 0; l < NUM_CIRCLES; l++)
        {
            int currentPosition = k * NUM_CIRCLES + l;
            targetArray[currentPosition].r = TO_BE_RANDOMIZED[k].radius;
            targetArray[currentPosition].d = TO_BE_RANDOMIZED[k].distance;
            angle = step * currentTarget - M_PI / 2;
            targetArray[currentPosition].x = centerX + TO_BE_RANDOMIZED[k].distance * cos(angle);
            targetArray[currentPosition].y = centerY + TO_BE_RANDOMIZED[k].distance * sin(angle);
            currentTarget = (currentTarget + (int)floor(stepNum)) % (NUM_CIRCLES);
            // printf("x: %f, y: %f, radius: %d, durchmesser: %d \n", targetArray[currentPosition].x, targetArray[currentPosition].y, targetArray[currentPosition].r, targetArray[currentPosition].d);
            // printf("currentTarget: %d, l: %d \n", currentTarget, l);
        }
        // printf(" \n");
    }
}

void circleDistribution(SDL_Renderer *renderer, int radius, int numCircles, int circleRadius)
{
    float angle, step;

    step = (2 * M_PI) / numCircles;

    for (int i = 0; i < numCircles; i++)
    {
        angle = step * i - M_PI / 2;

        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        /* TODO: Könnte man noch mit "filledCircleColor" ersetzten, dann könnte man die Farbe als konstante festlegen */
        filledCircleRGBA(renderer, x, y, circleRadius, 200 - i * 15, 200 - i * 15, 200 - i * 15, 255);
    }
}

