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


/* neue belegung: x,y(der des ziels), radius */
Target createTarget(int x, int y, int r)
{
    // if(DEBUG > 1) printf("createTarget()\n");
    Target target;

    target.r = r;
    target.y = HEIGHT / 2;
    target.x = WIDTH / 2;

    return target;
}



void circleDistribution(SDL_Renderer *renderer, int radius, int numCircles, int circleRadius)
{
    float angle, step;
    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;

    step = (2 * M_PI) / numCircles;

    for (int i = 0; i < numCircles; i++)
    {
        angle = step * i - M_PI / 2;
        /* das wchselt die richtung: */
        /* angle = -step * i - M_PI /2; */

        int x = cx + radius * cos(angle);
        int y = cy + radius * sin(angle);
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
