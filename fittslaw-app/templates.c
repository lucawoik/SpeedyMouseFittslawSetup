#include "main.h"


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
    int NUM_TUPELS = NUM_DISTANCE * NUM_RADIUS * NUM_ITERATIONS_PER_ID;
    Tupel TO_BE_RANDOMIZED[NUM_TUPELS];

    // create array of tuples with defined radius and distance
    for (int k = 0; k < NUM_ITERATIONS_PER_ID; k++)
    {
        for (int i = 0; i < NUM_DISTANCE; i++)
        {
            for (int j = 0; j < NUM_RADIUS; j++)
            {
                TO_BE_RANDOMIZED[NUM_RADIUS * (NUM_DISTANCE * k + i) + j].radius = TARGET_RADIUS[j];
                TO_BE_RANDOMIZED[NUM_RADIUS * (NUM_DISTANCE * k + i) + j].distance = TARGET_DISTANCE[i];
            }
        }
    }

    // randomize order of tuples
    for (int m = 0; m < NUM_TUPELS; m++)
    {
        Tupel *a = &TO_BE_RANDOMIZED[m];
        Tupel *b = &TO_BE_RANDOMIZED[rand() % (NUM_TUPELS)];
        swap(a, b);
    }

    float angle, step;
    step = (2 * M_PI) / NUM_CIRCLES;
    int stepNum = floor(NUM_CIRCLES/ 2);

    for (int k = 0; k < NUM_TUPELS; k++)
    {
        int currentTarget = 0;

        // fill target array with targets of same radius and same distance for a full circle (e.g. 9 circles)
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
    char buffer[12];
    sprintf(buffer, "%d", value);
    char *result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);

    return result;
}

// to-do: combine next methods
void circleDistribution(SDL_Renderer *renderer, int radius, int circleRadius, TTF_Font *font)
{
    int currentTarget = 0;
    float angle, step;
    step = (2 * M_PI) / NUM_CIRCLES;

    for (int i = 0; i < NUM_CIRCLES; i++)
    {
        // render circle
        angle = step * i - M_PI / 2;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        filledCircleRGBA(renderer, x, y, circleRadius, 170, 170, 170, 180);

        // render circle number
        char *text = intToString(currentTarget + 1);
        renderText(renderer, x, y, text, font);
        free(text);

        currentTarget = (currentTarget + NUM_CIRCLES-2) % (NUM_CIRCLES);
    }
}

void renderFeedback(SDL_Renderer *renderer, int radius, int circleRadius, TTF_Font *font, int successInCircle[], double elapsed_time)
/*
* renders the feedback screen, which includes:
* - the success rate of the last fitts law task "round" and
* - the last fitts law task "round" with a positive/negative feedback for each circle (indicating whether the circle was hit)
*/
{
    // render colored circles (based on success)
    int currentTarget = 0;
    float angle, step;
    step = (2 * M_PI) / NUM_CIRCLES;

    for (int i = 0; i < NUM_CIRCLES; i++)
    {
        angle = step * i - M_PI / 2;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);

        if (successInCircle[currentTarget])
        {
            filledCircleRGBA(renderer, x, y, circleRadius, 170, 170, 170, 255);
        }
        else
        {
            filledCircleRGBA(renderer, x, y, circleRadius, 250, 0, 0, 255);
        }

        currentTarget = (currentTarget + NUM_CIRCLES-2) % (NUM_CIRCLES);
    }

    // render feedback text based on success
    renderFeedbackText(renderer, font, successInCircle, elapsed_time);
}

void renderFeedbackText(SDL_Renderer *renderer, TTF_Font *font, int successInCircle[])
{
    // render text "x von y Zielen getroffen."
    char resultText[26];
    int checksum = calculateChecksum(successInCircle);
    char *checksumText = intToString(checksum);
    char partResultText[] = " von ";
    char *numCirclesText = intToString(NUM_CIRCLES);
    char zielenText[] = " Zielen";
    char getroffenText[] = "wurden getroffen.";

    strcpy(resultText, checksumText);
    strcat(resultText, partResultText);
    strcat(resultText, numCirclesText);
    strcat(resultText, zielenText);
    free(checksumText);
    free(numCirclesText);

    renderText(renderer, centerX, centerY - 15, resultText, font);
    renderText(renderer, centerX, centerY + 15, getroffenText, font);
}

void renderText(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font)
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
    SDL_DestroyTexture(texture);
}
