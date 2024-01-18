#include "main.h"

char EVENT_PATH[MAX_PATH_LENGTH] = "";
int PARTICIPANT_ID = 0;
int TRIAL = 0;
int LEVEL_OF_LATENCY = 0;

int iteration = 0;
int click_count = 0;
int click_count_total = 0;
int showing_feedback = 0;
double trial_time;
int travel_distance;
int lastX;
int lastY;
int isSetupTarget = 1;
int successInCircle[NUM_CIRCLES] = {0};

Trial current_trial;

int TARGET_RADIUS[NUM_RADIUS] = {15, 60, 100};       //{40, 60, 80}; min: 15 | max: 100
int TARGET_DISTANCE[NUM_DISTANCE] = {200, 325, 450}; //{200, 300, 400}; min: 200 | max: 400

// starting target
Target target = {centerX, centerY, 50, 200};

// int mouse_down = 0;

void finish()
{
    logClicks();
    SDL_Quit();
    exit(1);
}

void handleInput(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_MOUSEBUTTONDOWN && !showing_feedback) //TODO: showing_feedback flag does not work since the events are processed one after another...
        {
            if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
            {
                int mouseX, mouseY;

                SDL_GetMouseState(&mouseX, &mouseY);

                int success = checkCollision(mouseX, mouseY, &target);

                SDL_RenderPresent(renderer);

                if (isSetupTarget)
                {
                    // starting event logging right after setup target is clicked
                    startEventLogging();
                    isSetupTarget = 0;
                }
                else // only count the click if task is active
                {
                    // create click struct for logging
                    Click click = {click_count_total,
                                   millis(),
                                   target.r * 2,
                                   target.d,
                                   target.x,
                                   target.y,
                                   mouseX,
                                   mouseY,
                                   success,
                                   trial_time};

                    clicks[click_count_total] = click;
                    click_count_total++;
                    click_count++;

                    // circleNumber varies from 1 to number of circles
                    int circleNumber = iteration % NUM_CIRCLES;
                    successInCircle[circleNumber] = success;

                    // if a 2d fitts law task is completed
                    if (circleNumber == NUM_CIRCLES - 1)
                    {
                        stopEventLogging();
                        showing_feedback = 1;
                        // present feedback after ninth circle/click (or after NUM_CIRCLES clicks)
                        renderFeedback(renderer, target.d, target.r, font, successInCircle);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(800);
                        startEventLogging();
                        showing_feedback = 0;
                    }

                    iteration++;
                    if (iteration >= NUM_ITERATIONS)
                    {
                        finish();
                    }
                }

                click_count = 0;
                travel_distance = 0;
                trial_time = 0;
                target = createTarget(targetArray[iteration]);
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
    if (isSetupTarget)
    {
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
    if (strlen(argv[1]) <= MAX_PATH_LENGTH)
        if (sscanf(argv[1], "%255s", EVENT_PATH) == EOF)
            printf("incorrect event handle");
    if (sscanf(argv[2], "%d", &PARTICIPANT_ID) == EOF)
        printf("incorrect partcipant id");
    if (sscanf(argv[3], "%d", &TRIAL) == EOF)
        printf("incorrect trial id");
    if (sscanf(argv[4], "%d", &LEVEL_OF_LATENCY) == EOF)
        printf("incorrect level of latency");

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
    GetCurrentDir(font_path, FILENAME_MAX);
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

    // Init Logging - Open the event handler
    // Create a thread
    pthread_t loggingThread;
    int threadCreationResult = pthread_create(&loggingThread, NULL, initEventLogging, NULL);

    if (threadCreationResult != 0)
    {
        fprintf(stderr, "Error creating thread: %d\n", threadCreationResult);
        return 1; // Return an error code if thread creation fails
    }

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
