#include "main.h"

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

int isSetupTarget = 1;

Trial current_trial;

// too many permutations
// throw something out or simply cut after randomizing?
//int TARGET_RADIUS[3] = {75, 50, 25};
//int TARGET_DISTANCE[3] = {500, 400, 300};
//int TARGET_VELOCITY[3] = {200, 350, 500};

int TARGET_RADIUS[NUM_RADIUS] = {50, 30};
int TARGET_DISTANCE[NUM_DISTANCE] = {500};
int TARGET_VELOCITY[NUM_VELOCITY] = {200, 300, 400};
int TARGET_ANGLE[NUM_ANGLE] = {ANGLE_TOWARDS, ANGLE_DIAGONAL_TOWARDS, ANGLE_PERPENDICULAR, ANGLE_DIAGONAL_AWAY, ANGLE_AWAY};

// starting target
Target target = {WIDTH / 2 - 50, HEIGHT / 2 - 50, 100, 0, 0};



void finish()
{
    logClicks();
    logTrials();
    // write log files

    // print Trials
    /*
    for(int i = 0; i < NUM_ITERATIONS; i++)
    {
        printf("%d %ld | %d %d %d %d | %d %d %d %d | %f %d %d %d\n", trials[i].id, trials[i].timestamp, trials[i].w, trials[i].d, trials[i].v, trials[i].a, trials[i].x_target, trials[i].y_target, trials[i].x_cursor, trials[i].y_cursor, trials[i].time, trials[i].clicks, trials[i].travel_distance, trials[i].success);
    }
    */

    // print clicks
    //for(int i = 0; i < click_count_total; i++)
    //{
    //    printf("%d %ld | %d %d %d %d | %d %d %d %d | %d %d\n", 
    //            clicks[i].id,
    //            clicks[i].timestamp,
    //            clicks[i].w,
    //            clicks[i].d,
    //            clicks[i].v,
    //            clicks[i].a,
    //            clicks[i].x_target,
    //            clicks[i].y_target,
    //            clicks[i].x_cursor,
    //            clicks[i].y_cursor,
    //            clicks[i].distance,
    //            clicks[i].success);
    //}

    SDL_Quit();
    exit(1);
}

void handleInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
            {
                int mouseX, mouseY;

                SDL_GetMouseState(&mouseX, &mouseY);

                int success = checkCollision(mouseX, mouseY, &target);

                if(!isSetupTarget)
                {
                    // handle click
                    Click click = { click_count_total,
                                    millis(),
                                    target.r * 2,
                                    target.d,
                                    target.v,
                                    target.a,
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

                if(success)
                {
                    if(!isSetupTarget)
                    {
                        printf("success\n");
                        current_trial.time = trial_time;
                        current_trial.clicks = click_count;
                        current_trial.travel_distance = travel_distance;
                        current_trial.success = 1;

                        trials[iteration] = current_trial;

                        iteration++;   
                        if(iteration >= NUM_ITERATIONS) finish();
                    }

                    click_count = 0;
                    travel_distance = 0;
                    trial_time = 0;
                    isSetupTarget = 0;

                    target = createTarget(mouseX, mouseY, targetTemplates[iteration]);

                    current_trial = (Trial) {iteration,
                                             millis(),
                                             target.r * 2,
                                             target.d,
                                             target.v,
                                             target.a,
                                             target.x,
                                             target.y,
                                             mouseX,
                                             mouseY,
                                             0,
                                             0,
                                             0,
                                             0};

                    // fixed: was caused by the cursor being too far away from the base line
                    //if(target.x < -2000)
                    //{
                    //    printf("problem here!\n");
                    //    finish();
                    //}

                    if(DEBUG > 1) printf("target created: x %f y %f r %d vX %f vY %f\n", target.x, target.y, target.r, target.vX, target.vY);

                }
            }
        }

        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
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

void render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    filledCircleColor(renderer, target.x, target.y, target.r, TARGET_COLOR);

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

    target.x += target.vX * deltaTime;
    target.y += target.vY * deltaTime;

    if( target.x < -target.r * 2 ||
        target.x > WIDTH + (target.r * 2) ||
        target.y < -target.r * 2 ||
        target.y > HEIGHT + (target.r * 2))
    {
        printf("failed\n");
        current_trial.time = 0;
        current_trial.clicks = click_count;
        current_trial.travel_distance = travel_distance;
        current_trial.success = 0;
        trials[iteration] = current_trial;

        target = (Target) {WIDTH / 2 - 50, HEIGHT / 2 - 50, 100, 0, 0};
        click_count = 0;
        travel_distance = 0;
        trial_time = 0;
        isSetupTarget = 1;
        
        iteration++;
        if(iteration >= NUM_ITERATIONS) finish();
    }
}

int main(int argc, char** argv)
{
    if(sscanf(argv[1], "%d", &PARTICIPANT_ID) == EOF) printf("incorrect partcipant id");
    if(sscanf(argv[2], "%d", &EXPERIMENT) == EOF) printf("incorrect trial id");
    if(sscanf(argv[3], "%d", &LATENCY_CLICK_MIN) == EOF) printf("incorrect latency click min");
    if(sscanf(argv[4], "%d", &LATENCY_CLICK_MAX) == EOF) printf("incorrect latency click max");
    if(sscanf(argv[5], "%d", &LATENCY_MOVE_MIN) == EOF) printf("incorrect latency move min");
    if(sscanf(argv[6], "%d", &LATENCY_MOVE_MAX) == EOF) printf("incorrect latency move max");

    double timer;
    double deltaTime;
    SDL_Window* window;
    SDL_Renderer* renderer;

    srand(time(0));

    SDL_Init(SDL_INIT_EVERYTHING);

    initTargetTemplates();

    window = SDL_CreateWindow(__FILE__, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    //thread inputThread(handleInput);

    while(1)
    {
        deltaTime = (millis() - timer) / 1000;
        timer = millis();

        update(deltaTime);
        handleInput();
        render(renderer);
    }

    SDL_Quit();

    return 0;
}
