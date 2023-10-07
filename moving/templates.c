#include "main.h"

//// swap array elements
//void swap(TargetTemplate **a, TargetTemplate **b)
//{
//    //if(DEBUG > 1) printf("swap()\n");
//	TargetTemplate *temp = *a;
//    *a = *b;
//    *b = temp;
//}


TargetTemplate createTargetTemplate(int r, int d, int v, int a)
{
    //if(DEBUG > 1) printf("createTargetTemplate()\n");

    TargetTemplate targetTemplate;
    targetTemplate.r = r;
    targetTemplate.d = d;
    targetTemplate.v = v;
    targetTemplate.a = a;
    return targetTemplate;
}

Target createTarget(int x, int y, TargetTemplate *targetTemplate)
{
    //if(DEBUG > 1) printf("createTarget()\n");

    //TargetTemplate *targetTemplate = targetTemplates[iteration];
    Target target;

    int yDist = abs((HEIGHT / 2) - y);

    // if the mouse is too far away from the line, create a dummy target
    if(yDist >= targetTemplate->d)
    {
        target = (Target) {WIDTH / 2 - 50, HEIGHT / 2 - 50, 100, 0, 0};
        isSetupTarget = 1;
        return target;
    }
    int xDist = sqrt(pow(targetTemplate->d, 2) - pow(yDist, 2));

    //printf("%d %d %d %d\n", x, y, xDist, yDist);

    //printf("pow: %f\n", pow(targetTemplate->d, 2));
    //printf("multi: %f\n", targetTemplate->d * targetTemplate->d);
    //printf("pythagoras: %f\n", sqrt(pow(targetTemplate->d, 2) - pow(yDist, 2)));
    //printf("d: %d\n", targetTemplate->d);
    //printf("yDist: %d\n", yDist);
    //printf("xDist: %d\n", xDist);

    target.y = HEIGHT / 2;
    target.x = x < WIDTH / 2 ? x + xDist : x - xDist;
    target.r = targetTemplate->r;
    target.d = targetTemplate->d;
    target.v = targetTemplate->v;
    target.a = targetTemplate->a;

    if(targetTemplate->v > 0)
    {
        switch(targetTemplate->a)
        {
        case ANGLE_TOWARDS: // towards
            target.vX = targetTemplate->v;
            target.vY = 0;
            if(x < target.x) target.vX *= -1;
            break;
        case ANGLE_DIAGONAL_TOWARDS: // diagonally towards
            target.vX = sqrt(pow(targetTemplate->v, 2) / 2);
            target.vY = target.vX;
            if(x < target.x) target.vX *= -1;
            if(rand() % 2 == 0) target.vY *= -1;
            break;
        case ANGLE_PERPENDICULAR: // perpendicular
            target.vX = 0;
            target.vY = targetTemplate->v;
            if(rand() % 2 == 0) target.vY *= -1;
            break;
        case ANGLE_DIAGONAL_AWAY: // diagonally away
            target.vX = sqrt(pow(targetTemplate->v, 2) / 2);
            target.vY = target.vX;
            if(x > target.x) target.vX *= -1;
            if(rand() % 2 == 0) target.vY *= -1;
            break;
        case ANGLE_AWAY: // away
            target.vX = targetTemplate->v;
            target.vY = 0;
            if(x > target.x) target.vX *= -1;
            break;
        /* case ANGLE_NONE:
	    //printf("static\n");
	    target.vX = 0;
	    target.vY = 0;
            break;
            */
        } 
    }
    else
    {
	target.vX = 0;
	target.vY = 0;
    }

    //printf("template %f %f %d\n", target.vX, target.vY, targetTemplate->a);

    return target;
}

void initTargetTemplates()
{
    if(DEBUG > 1) printf("initTargetTemplates()\n");

    int i = 0;
    for(int r = 0; r < NUM_RADIUS; r++)
    {
        for(int d = 0; d < NUM_DISTANCE; d++)
        {
            for(int v = 0; v < NUM_VELOCITY; v++)
            {
                for(int a = 0; a < NUM_ANGLE; a++)
                {
                    /* staticTargetTemplates[i] = createTargetTemplate(TARGET_RADIUS[r], TARGET_DISTANCE[d], 0, 5); */
                    movingTargetTemplates[i] = createTargetTemplate(TARGET_RADIUS[r], TARGET_DISTANCE[d], TARGET_VELOCITY[v], a);
                    i++;
                }
            }
        }
    }

    if(DEBUG > 1) printf("templates created\n");

    //for(int j = 0; j < 10; j++)
    //{
    //    TargetTemplate t = movingTargetTemplates[j];
    //    printf("r %d d %d v %d a %d\n", t.r, t.d, t.v, t.a);
    //}

	for(int j = 0; j < NUM_ITERATIONS / 2; j++)
	{
        //printf("%d %f -> ", staticTargetTemplates[j].r, staticTargetTemplates[j].d);
        /* TargetTemplate *a = &staticTargetTemplates[j];
        TargetTemplate *b = &staticTargetTemplates[rand() % (NUM_ITERATIONS / 2)]; */
        //printf("before: a: %d %d, b: %d %d, r: %d %d, d: %f %f\n", a, &a, b, &b, a->r, b->r, a->d, b->d);
		/* swap(a, b); */
        //printf(" after: a: %d %d, b: %d %d, r: %d %d, d: %f %f\n", a, &a, b, &b, a->r, b->r, a->d, b->d);
        //printf("%d %f\n", staticTargetTemplates[j].r, staticTargetTemplates[j].d);
        TargetTemplate *c = &movingTargetTemplates[j];
        TargetTemplate *d = &movingTargetTemplates[rand() % (NUM_ITERATIONS)];
		swap(c, d);
		//swap(&movingTargetTemplates[j], &movingTargetTemplates[rand() % NUM_ITERATIONS]);
	}

    //for(int j = 0; j < NUM_ITERATIONS / 2; j++)
    //{
    //    TargetTemplate t = movingTargetTemplates[j];
    //    printf("r %d d %d v %d a %d\n", t.r, t.d, t.v, t.a);
    //}

    if(DEBUG > 1) printf("shuffled\n");


    /* TODO: kann man dann auch einfach gleichsetzten ohne for?! */
    for(int j = 0; j < NUM_ITERATIONS / 2; j++)
    {
        /* targetTemplates[j * 2] = &staticTargetTemplates[j]; */
        targetTemplates[j] = &movingTargetTemplates[j];
    }

    //for(int j = 0; j < NUM_ITERATIONS; j++)
    //{
    //    printf("%d %d\n", j, targetTemplates[j]->v);
    //}

    if(DEBUG > 1) printf("end initTargetTemplates()\n");
}
