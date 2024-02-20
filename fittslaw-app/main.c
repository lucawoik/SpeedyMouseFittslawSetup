#include "main.h"

char EVENT_PATH[MAX_PATH_LENGTH] = "";
int PARTICIPANT_ID = 0;
int TRIAL = 0;
int LEVEL_OF_LATENCY = 0;
int IS_TEST_MODE = 0;

int iteration = 0;
int click_count_total = 0;
double trial_time;
int travel_distance;
int lastX;
int lastY;
int isSetupTarget = 1;
int measureLatency = 0;
int successInCircle[NUM_CIRCLES] = {0};
int isLogging = 0;

Trial current_trial;

int TARGET_RADIUS[NUM_RADIUS] = {15, 60, 100};       //{40, 60, 80}; min: 15 | max: 100
int TARGET_DISTANCE[NUM_DISTANCE] = {200, 325, 450}; //{200, 300, 400}; min: 200 | max: 400

// starting target
Target measuringTarget = {70, 70, 200, 0};
Target measuringTargetRight = {WIDTH - 50, HEIGHT - 50, 200, 0};
Target target = {centerX, centerY, 50, 200};
Target lastTarget = {centerX, centerY, 50, 200};

// time variables for calculation of duration of feedback
const Uint32 displayFeedbackTime = 700;
Uint32 elapsedTime = 700;
Uint32 startTime = 0;

void finish()
{
    logClicks();
    SDL_Quit();
    exit(1);
}

void handleInput(SDL_Renderer *renderer)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
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

        if (elapsedTime >= displayFeedbackTime)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
                {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    int success = checkCollision(mouseX, mouseY, &target);

                    SDL_RenderPresent(renderer);

                    if (isSetupTarget)
                    {
                        isSetupTarget = 0;
                    }
                    else if (measureLatency)
                    {
                        measureLatency = 0;
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

                        // circleNumber varies from 1 to number of circles
                        int circleNumber = iteration % NUM_CIRCLES;
                        successInCircle[circleNumber] = success;

                        // if a 2d fitts law task is completed
                        if (circleNumber == NUM_CIRCLES - 1)
                        {
                            // get starting time of feedback screen for calculation of how long it is displayed
                            startTime = SDL_GetTicks();
                        }

                        iteration++;
                        if (iteration >= NUM_ITERATIONS)
                        {
                            finish();
                        }
                    }

                    travel_distance = 0;
                    trial_time = 0;
                    lastTarget = target;
                    target = createTarget(targetArray[iteration]);
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                measureLatency = 0; // 1;
            }
        }
    }
}

void render(SDL_Renderer *renderer, TTF_Font *fontNumbers, TTF_Font *fontFeedback)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // calculate how long the feedback screen has been displayed
    Uint32 currentTime = SDL_GetTicks();
    elapsedTime = currentTime - startTime;
    if (elapsedTime < displayFeedbackTime)
    {
        renderFeedback(renderer, lastTarget.d, lastTarget.r, fontFeedback, successInCircle);
        if (isLogging)
        {
            logEvents();
            isLogging = 0;
        }
    }
    else
    {
        circleDistribution(renderer, target.d, target.r, fontNumbers);
        if (!isLogging && !isSetupTarget)
        {
            isLogging = 1;
        }
    }

    // render first circle
    if (isSetupTarget)
    {
        filledCircleColor(renderer, target.x, target.y, target.r, TARGET_COLOR);
    }
    if (measureLatency)
    {
        filledCircleColor(renderer, measuringTarget.x, measuringTarget.y, measuringTarget.r, TARGET_COLOR);
        filledCircleColor(renderer, measuringTargetRight.x, measuringTargetRight.y, measuringTarget.r, TARGET_COLOR);
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
    if (sscanf(argv[5], "%d", &IS_TEST_MODE) == EOF)
        printf("incorrect test mode flag");

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

    // Init thread for mouse manipulation
    pthread_t eventThread;
    int threadCreationResult = pthread_create(&eventThread, NULL, manipulateMouseEvents, NULL);

    if (threadCreationResult != 0)
    {
        fprintf(stderr, "Error creating thread: %d\n", threadCreationResult);
        return 1; // Return an error code if thread creation fails
    }

    // Init Logging - Open the event handler
    // Create a thread
    /* pthread_t loggingThread;
    int threadCreationResult = pthread_create(&loggingThread, NULL, initEventLogging, NULL);

    if (threadCreationResult != 0)
    {
        fprintf(stderr, "Error creating thread: %d\n", threadCreationResult);
        return 1; // Return an error code if thread creation fails
    } */

    while (1)
    {
        deltaTime = (micros() - timer) / 1000000.0;
        timer = micros();

        update(deltaTime);
        handleInput(renderer);
        render(renderer, fontNumbers, fontFeedback);
        // usleep(2000);
        SDL_RenderPresent(renderer);
    }
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}