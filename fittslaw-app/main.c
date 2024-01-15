#include "main.h"

char EVENT_PATH[MAX_PATH_LENGTH] = "";
int PARTICIPANT_ID = 0;
int EXPERIMENT = 0;
int LATENCY_CLICK_MIN = 0;
int LATENCY_CLICK_MAX = 0;
int LATENCY_MOVE_MIN = 0;
int LATENCY_MOVE_MAX = 0;

int iteration = 0;
int click_count = 0;
int click_count_total = 0;
double trial_time;
int travel_distance;
int lastX;
int lastY;
/* TODO: unnötig */
int isSetupTarget = 1;
int successInCircle[9] = {0};

Trial current_trial;

/* int TARGET_RADIUS[NUM_RADIUS] = {20, 40, 60, 100}; */
int TARGET_RADIUS[NUM_RADIUS] = {40, 60, 80};        //{30, 50, 70};
int TARGET_DISTANCE[NUM_DISTANCE] = {200, 300, 400}; //{400, 500, 600};

// starting target
/* TODO: Wo soll erstes probetarget sein? */
Target target = {150, 150, 100, 200};

int mouse_down = 0;

void finish()
{
    logClicks();
    logTrials();
    SDL_Quit();
    exit(1);
}
/* TODO: noch mal anschaun ob das mit den rausgenommenen Werten wie distanc und velocity
immernoch die richtigen sachen berechnet */
void handleInput(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
            {
                mouse_down = 1;

                int mouseX, mouseY;

                SDL_GetMouseState(&mouseX, &mouseY);

                int success = checkCollision(mouseX, mouseY, &target);

                SDL_RenderPresent(renderer);
                SDL_Delay(200);

                if (isSetupTarget){
                    startEventLogging();
                }

                /* TODO: setup nicht mehr vorhanden, aber das erste wird ja nicht gezählt also vllt das hier rausnehmen?! */
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

                /* TODO: (Wie oben?) setup nicht mehr vorhanden, aber das erste wird ja nicht gezählt also vllt das hier rausnehmen?! */
                if (!isSetupTarget)
                {
                    int circleNumber = iteration % 9;
                    successInCircle[circleNumber] = success;

                    // present feedback after ninth circle
                    if (circleNumber == 8)
                    {
                        stopEventLogging();
                        renderFeedback(renderer, target.d, NUM_CIRCLES, target.r, font, successInCircle);
                        SDL_RenderPresent(renderer);
                        // implement with delay or is a new circle presented by clicking somewhere?
                        SDL_Delay(800);
                        startEventLogging();
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
                /* TODO: hier werden die neuen targets generiert
                könnte man da also nicht einfach das machen:
                target = targetArray[iteration];
                ?
                */
                target = createTarget(targetArray[iteration]);

                // printf("main %f %f\n", target.x, target.y);
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

                /* if (DEBUG > 1)
                    printf("target created: x %f y %f r %d vX %f vY %f\n", target.x, target.y, target.r, target.vX, target.vY);
            */
            }
        }

        /*         if (event.type == SDL_MOUSEBUTTONUP)
                {
                    if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
                    {
                mouse_down = 0;
                }
            } */

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
    circleDistribution(renderer, target.d, NUM_CIRCLES, target.r, font);

    /* funktion die feedbackcircle malt
        - nur wenn nicht seit dem letzt click 200ms vergangen sind
            (timpstam beim klick speichern  => !(currentTime>= lastTime+200ms) )
      */

    // circle in bottom right corner used to measure end to end latency
    if (!mouse_down)
    {
        filledCircleColor(renderer, 1900, 1000, 100, TARGET_COLOR);
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
    if (sscanf(argv[3], "%d", &EXPERIMENT) == EOF)
        printf("incorrect trial id");
    if (sscanf(argv[4], "%d", &LATENCY_CLICK_MIN) == EOF)
        printf("incorrect latency click min");
    if (sscanf(argv[5], "%d", &LATENCY_CLICK_MAX) == EOF)
        printf("incorrect latency click max");
    if (sscanf(argv[6], "%d", &LATENCY_MOVE_MIN) == EOF)
        printf("incorrect latency move min");
    if (sscanf(argv[7], "%d", &LATENCY_MOVE_MAX) == EOF)
        printf("incorrect latency move max");

    double timer;
    double deltaTime;
    SDL_Window *window;
    SDL_Renderer *renderer;
    char *font_path;

    srand(time(0));

    SDL_Init(SDL_INIT_EVERYTHING);

    createTargetArray();

    window = SDL_CreateWindow(__FILE__, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Init TTF.
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
    font_path = "font/arial.ttf";
    TTF_Init();
    TTF_Font *fontNumbers = TTF_OpenFont(font_path, 48);
    TTF_Font *fontFeedback = TTF_OpenFont(font_path, 28);

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
