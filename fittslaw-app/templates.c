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
        }
    }

    for (int m = 0; m < NUM_TUPELS; m++)
    {
        Tupel *a = &TO_BE_RANDOMIZED[m];
        Tupel *b = &TO_BE_RANDOMIZED[rand() % (NUM_TUPELS)];
        swap(a, b);
    }

    float angle, step;
    step = (2 * M_PI) / NUM_CIRCLES;
    int stepNum = floor(NUM_CIRCLES / 2);

    for (int k = 0; k < NUM_TUPELS; k++)
    {
        int currentTarget = 0;

        for (int l = 0; l < NUM_CIRCLES; l++)
        {
            int currentPosition = k * NUM_CIRCLES + l;
            targetArray[currentPosition].r = TO_BE_RANDOMIZED[k].radius;
            targetArray[currentPosition].d = TO_BE_RANDOMIZED[k].distance;
            angle = step * currentTarget - M_PI / 2;
            targetArray[currentPosition].x = centerX + TO_BE_RANDOMIZED[k].distance * cos(angle);
            targetArray[currentPosition].y = centerY + TO_BE_RANDOMIZED[k].distance * sin(angle);
            currentTarget = (currentTarget + (int)floor(stepNum)) % (NUM_CIRCLES);
        }
    }
}

char *intToString(int value)
{
    char buffer[20];
    sprintf(buffer, "%d", value);
    char *result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);

    return result;
}

void circleDistribution(SDL_Renderer *renderer, int radius, int numCircles, int circleRadius, TTF_Font *font)
{
    float angle, step;

    // do not hardcode later
    int order[] = {1, 8, 6, 4, 2, 9, 7, 5, 3};

    step = (2 * M_PI) / numCircles;

    for (int i = 0; i < numCircles; i++)
    {
        angle = step * i - M_PI / 2;

        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        filledCircleRGBA(renderer, x, y, circleRadius, 170, 170, 170, 255);

        char *text = intToString(order[i]);
        renderNumbers(renderer, x, y, text, font);
    }
}

void renderFeedback(SDL_Renderer *renderer, int radius, int numCircles, int circleRadius, TTF_Font *font, int successInCircle[])
{
    float angle, step;
    int order[] = {1, 8, 6, 4, 2, 9, 7, 5, 3};

    step = (2 * M_PI) / numCircles;

    for (int i = 0; i < numCircles; i++)
    {
        angle = step * i - M_PI / 2;

        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);

        if (successInCircle[order[i] - 1])
        {
            filledCircleRGBA(renderer, x, y, circleRadius, 0, 250, 0, 255);
        }
        else
        {
            filledCircleRGBA(renderer, x, y, circleRadius, 250, 0, 0, 255);
        }
    }

    int checksum = calculateChecksum(successInCircle, sizeof(&successInCircle));
    char *text = strcat(intToString(checksum), " von 9 Zielen getroffen.");
    renderNumbers(renderer, WIDTH / 2, HEIGHT / 2, text, font);
}

void renderNumbers(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font)
{
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color textColor = {0, 0, 0, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    SDL_Rect rect = {x - text_width / 2, y - text_height / 2, text_width, text_height};

    SDL_RenderCopy(renderer, texture, NULL, &rect);
}
