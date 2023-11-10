#include "main.h"

//// swap array elements
/* Todo: ist vllt notwendig in initTargetTemplates */
// void swap(TargetTemplate **a, TargetTemplate **b)
//{
//     //if(DEBUG > 1) printf("swap()\n");
//	TargetTemplate *temp = *a;
//     *a = *b;
//     *b = temp;
// }

/* neue belegung: x,y(der des ziels), radius, distance to the center */
/* Target createTarget(int x, int y, int r, int d) */
/* ??? ist das so richtig? da weiter probieren und dann mal bilden lassen! */
Target createTarget(Target t)
{
    // if(DEBUG > 1) printf("createTarget()\n");
    Target target;

    target.r = t.r;
    target.y = t.y;
    target.x = t.x;
    target.d = t.d;

    return target;
}

/* test */

/* TARGET_RADIUS, TARGET_DISTANCE */
void createTargetArray()
{
/* toBeRandomized */
    Tupel TO_BE_RANDOMIZED[NUM_DISTANCE * NUM_RADIUS];

    for (int i = 0; i < NUM_DISTANCE; i++)
    {
        for (int j = 0; j < NUM_RADIUS; j++)
        {
            TO_BE_RANDOMIZED[NUM_RADIUS * i + j].radius = TARGET_RADIUS[j];
            TO_BE_RANDOMIZED[NUM_RADIUS * i + j].distance = TARGET_DISTANCE[i];
        }
    }
    
    /* TODO: randiómisieren des  toBeRandomized[] !!! */

    float angle, step;
    step = (2 * M_PI) / NUM_CIRCLES;

    for (int k = 0; k < sizeof(TO_BE_RANDOMIZED); k++)
    {
        for (int l = 0; l < NUM_CIRCLES; l++)
        {
            int currentPosition = k * NUM_CIRCLES + l;
            targetArray[currentPosition].r = TO_BE_RANDOMIZED[k].radius;
            targetArray[currentPosition].d = TO_BE_RANDOMIZED[k].distance;
            angle = step * l - M_PI / 2;
            targetArray[currentPosition].x = centerX + TARGET_DISTANCE[k] * cos(angle);
            targetArray[currentPosition].y = centerY + TARGET_DISTANCE[k] * sin(angle);
        }
    }
}

/* test ende */

void circleDistribution(SDL_Renderer *renderer, int radius, int numCircles, int circleRadius)
{
    float angle, step;

    step = (2 * M_PI) / numCircles;

    for (int i = 0; i < numCircles; i++)
    {
        angle = step * i - M_PI / 2;
        /* das wchselt die richtung: */
        /* angle = -step * i - M_PI /2; */

        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        /* TODO: Könnte man noch mit "filledCircleColor" ersetzten, dann könnte man die Farbe als konstante festlegen */
        filledCircleRGBA(renderer, x, y, circleRadius, 200 - i * 15, 200 - i * 15, 200 - i * 15, 255);
    }
}

// void initTargetTemplates()
// {
//     if (DEBUG > 1)
//         printf("initTargetTemplates()\n");

//     int i = 0;
//     for (int r = 0; r < NUM_RADIUS; r++)
//     {
//         staticTargetTemplates[i] = createTargetTemplate(TARGET_RADIUS[r]);
//         i++;
//     }

//     if (DEBUG > 1)
//         printf("templates created\n");

//     // for(int j = 0; j < 10; j++)
//     //{
//     //     TargetTemplate t = movingTargetTemplates[j];
//     //     printf("r %d d %d v %d a %d\n", t.r, t.d, t.v, t.a);
//     // }

//     /* TODO: wahrschienlihc auch nicht notwendig, da ja eine feste reihenfolge der orte besteht.
//     Man könnte höchstend  die Radiie und */
//     for (int j = 0; j < NUM_ITERATIONS / 2; j++)
//     {
//         // printf("%d %f -> ", staticTargetTemplates[j].r, staticTargetTemplates[j].d);
//         TargetTemplate *a = &staticTargetTemplates[j];
//         TargetTemplate *b = &staticTargetTemplates[rand() % (NUM_ITERATIONS / 2)];
//         // printf("before: a: %d %d, b: %d %d, r: %d %d, d: %f %f\n", a, &a, b, &b, a->r, b->r, a->d, b->d);
//         swap(a, b);
//     }

//     // for(int j = 0; j < NUM_ITERATIONS / 2; j++)
//     //{
//     //     TargetTemplate t = movingTargetTemplates[j];
//     //     printf("r %d d %d v %d a %d\n", t.r, t.d, t.v, t.a);
//     // }

//     if (DEBUG > 1)
//         printf("shuffled\n");

//     for (int j = 0; j < NUM_ITERATIONS / 2; j++)
//     {
//         targetTemplates[j] = &staticTargetTemplates[j];
//         /* targetTemplates[j * 2 + 1] = &movingTargetTemplates[j]; */
//     }

//     // for(int j = 0; j < NUM_ITERATIONS; j++)
//     //{
//     //     printf("%d %d\n", j, targetTemplates[j]->v);
//     // }

//     if (DEBUG > 1)
//         printf("end initTargetTemplates()\n");
// }
