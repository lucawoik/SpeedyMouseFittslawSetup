#include "main.h"

int PARTICIPANT_ID = 0;
int EXPERIMENT = 0;
int LATENCY = 0;

int iteration = 0;
int click_count = 0;
int click_count_total = 0;
double trial_time;
int travel_distance;
int lastX;
int lastY;
int isSetupTarget = 1;
int successInCircle[NUM_CIRCLES] = {0};

Trial current_trial;

int TARGET_RADIUS[NUM_RADIUS] = {15, 60, 100}; //{40, 60, 80}; min: 15 | max: 100
int TARGET_DISTANCE[NUM_DISTANCE] = {200, 325, 450}; //{200, 300, 400}; min: 200 | max: 400

// starting target
Target target = {centerX, centerY, 50, 200};

// int mouse_down = 0;

// variables for measuring duration of one 2d fitts law task
struct timespec start_time, end_time;
double elapsed_time;


void finish()
{
    logClicks();
    logTrials();
    SDL_Quit();
    exit(1);
}

void handleInput(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
            {
                // mouse_down = 1;

                int mouseX, mouseY;

                SDL_GetMouseState(&mouseX, &mouseY);

                int success = checkCollision(mouseX, mouseY, &target);

                SDL_RenderPresent(renderer);

                if (!isSetupTarget)
                {
                    // handle click
                    Click click = {click_count_total,
                                   millis(),
                                   target.r * 2,
                                   target.d,
                                   target.x,
                                   target.y,
                                   mouseX,
                                   mouseY,
                                   max(calculateDistance(target.x, target.y, mouseX, mouseY) - target.r, 0),
                                   success};

                    clicks[click_count_total] = click;
                    click_count_total++;
                }

                click_count++;

                if (!isSetupTarget)
                {
                    // circleNumber varies from 1 to number of circles
                    int circleNumber = iteration % NUM_CIRCLES;
                    successInCircle[circleNumber] = success;

                    // Record the starting time after first click
                    if (circleNumber == 1)
                    {
                        clock_gettime(CLOCK_MONOTONIC, &start_time);
                    }

                    // if a 2d fitts law task is completed
                    if (circleNumber == NUM_CIRCLES - 1)
                    {
                        // is trial_time the same as elapsed_time??
                        // calculate task completion time
                        clock_gettime(CLOCK_MONOTONIC, &end_time);
                        elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                                        (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
                        elapsed_time = elapsed_time / 1000;

                        // present feedback after ninth circle/click (or after NUM_CIRCLES clicks)
                        renderFeedback(renderer, target.d, target.r, font, successInCircle, elapsed_time);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(800);
                    }

                    current_trial.time = trial_time;
                    current_trial.clicks = click_count;
                    current_trial.travel_distance = travel_distance;
                    current_trial.success = 1;
                    trials[iteration] = current_trial;
                    iteration++;
                    if (iteration >= NUM_ITERATIONS)
                    {
                        finish();
                    }
                }

                click_count = 0;
                travel_distance = 0;
                trial_time = 0;
                isSetupTarget = 0;
                
                target = createTarget(targetArray[iteration]);

                current_trial = (Trial){iteration,
                                        millis(),
                                        target.r * 2,
                                        target.d,
                                        target.x,
                                        target.y,
                                        mouseX,
                                        mouseY,
                                        0,
                                        0,
                                        0,
                                        0};
            }
        }

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_q:
                finish();
                SDL_Quit();
                exit(1);
                break;
            }
        }
    }
}

void render(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    circleDistribution(renderer, target.d, target.r, font);
    // render first circle
    if (isSetupTarget){
        filledCircleColor(renderer, target.x, target.y, target.r, TARGET_COLOR);
    }

    SDL_RenderPresent(renderer);
}

void update(double deltaTime)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    travel_distance += calculateDistance(mouseX, mouseY, lastX, lastY);
    trial_time += deltaTime;

    lastX = mouseX;
    lastY = mouseY;
}

int main(int argc, char **argv)
{
    if (sscanf(argv[1], "%d", &PARTICIPANT_ID) == EOF)
        printf("incorrect partcipant id");
    if (sscanf(argv[2], "%d", &EXPERIMENT) == EOF)
        printf("incorrect trial id");
    if (sscanf(argv[3], "%d", &LATENCY) == EOF)
        printf("incorrect latency");

    double timer;
    double deltaTime;
    SDL_Window *window;
    SDL_Renderer *renderer;
    // char *font_path;

    srand(time(0));

    SDL_Init(SDL_INIT_EVERYTHING);

    createTargetArray();

    window = SDL_CreateWindow(__FILE__, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Init TTF.
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
    TTF_Init();
    
    char font_path[FILENAME_MAX];
    GetCurrentDir( font_path, FILENAME_MAX );
    strcat(font_path, "/fittslaw-app/font/arial.ttf");
    TTF_Font *fontNumbers = TTF_OpenFont(font_path, 36);
    TTF_Font *fontFeedback = TTF_OpenFont(font_path, 24);

    if (fontNumbers == NULL || fontFeedback == NULL)
    {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    system("xsetroot -cursor_name arrow");

    while (1)
    {
        deltaTime = (micros() - timer) / 1000000.0;
        timer = micros();

        update(deltaTime);
        handleInput(renderer, fontFeedback);
        render(renderer, fontNumbers);
        // usleep(2000);

        SDL_RenderPresent(renderer);
    }
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
