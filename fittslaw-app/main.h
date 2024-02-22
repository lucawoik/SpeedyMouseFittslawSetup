#ifndef __fitts_main__
#define __fitts_main__

#include <stdio.h>
#include <unistd.h>
#define GetCurrentDir getcwd
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include "tensorflow/c/c_api.h"
#include <libevdev/libevdev.h>

#define WIDTH 1920
#define HEIGHT 1080

#define NUM_RADIUS 3
#define NUM_DISTANCE 3

/* #define ANGLE_NONE 5 */

/* number of circles in the circle */
#define NUM_CIRCLES 9

#define NUM_ITERATIONS_PER_ID 4

#define MAX_CLICKS 10000
#define MAX_EVENTS 1000000

#define DEBUG 0

#define TARGET_COLOR 0xFF000000

#define TOTAL_NUM_CIRCLES NUM_CIRCLES *NUM_ITERATIONS_PER_ID

#define NUM_ITERATIONS NUM_RADIUS *NUM_DISTANCE *TOTAL_NUM_CIRCLES

/* TODO: hier könnte man noch nach static und mooving unterscheiden */
#define LOG_PATH "log"

#define centerX WIDTH / 2
#define centerY HEIGHT / 2 + 13

#define MAX_PATH_LENGTH 256

// Predict.c
#define INTERVAL_LENGTH 5
#define BUFFER_LENGTH 5
#define X_RANGE 73.0f
#define X_MIN -31.0f
#define Y_RANGE 59.0f
#define Y_MIN -28.0f
#define PREDICTION_ACTIVE 0
#define DELAY_MS 500

extern char EVENT_PATH[MAX_PATH_LENGTH];
extern char MODEL_DIR[MAX_PATH_LENGTH];
extern int PARTICIPANT_ID;
extern int TRIAL;
extern int LEVEL_OF_LATENCY;
extern int IS_TEST_MODE;

extern int TARGET_RADIUS[NUM_RADIUS];
extern int TARGET_DISTANCE[NUM_DISTANCE];

extern int isSetupTarget;
extern int click_count_total;
extern int positionsLogged;
extern void *manipulateMouseEvents(void *arg);

extern long intervalCounter;

extern int isLogging;

/* TODO: nicht mehr notwendig, aber noch in den Logs drin */
/* static const char *ANGLE_STRING[] = {
    "towards", "towards_diagonal", "perpendicular", "away_diagonal", "away", "none",
}; */

typedef struct
{
    double x;
    double y;
    int r;
    int d;
} Target;

/* TODO: Anpassen! */
/*     int a; */
/*     int v; */
typedef struct
{
    int id;
    long timestamp;
    int w;
    int d;

    int x_target;
    int y_target;
    int x_cursor;
    int y_cursor;
    double time; // in ms
    int clicks;
    int travel_distance;
    int success;
} Trial;

/*   int a;*/
// int v;
typedef struct
{
    int id;
    long timestamp;
    int w;
    int d;

    int x_target;
    int y_target;
    int x_cursor;
    int y_cursor;
    int success;
    double completion_time; // in ms
} Click;

typedef struct
{
    int id;
    long timestamp;
    int x;
    int y;
} MousePosition;

typedef struct
{
    int radius;
    int distance;
} Tupel;

// for saving a resampled event
typedef struct
{
    long timestamp;
    int intervalX;
    int intervalY;
    float predictedX;
    float predictedY;
} LogEvent;

typedef struct
{
    float x;
    float y;
} NormalizedEvent;

typedef struct
{
    int x;
    int y;
    int delay_ms;
} DelayedEvent;


// Circular buffer to save all past resampled events
typedef struct
{
    NormalizedEvent events[BUFFER_LENGTH];
    int front;
} CircularBuffer;

Target targetArray[NUM_ITERATIONS];

int fd;
int currently_logging;
int eventLogHeadWritten;

Trial trials[NUM_ITERATIONS];
Click clicks[MAX_CLICKS];
LogEvent loggedEvents[MAX_EVENTS];
pthread_mutex_t loggedEventsMutex;
MousePosition positions[MAX_CLICKS];
int positionsLogged;

int eventCount;

/* TODO: macht das so noch Sinn odr kann man einfach die Targets direkt aus dem Array nehmen?! */
Target createTarget(Target t);

// utils
void swap(Tupel *a, Tupel *b);

long millis();

long long micros();

int min(int a, int b);

int max(int a, int b);

int checkCollision(int x, int y, Target *t);

int calculateDistance(int x1, int y1, int x2, int y2);

int calculateChecksum(int array[]);

float normalize(float value, char axis);

float denormalize(float value, char axis);

// log
void logClicks();

void appendEvents(int intervalX, int intervalY, float predX, float predY);

void logEvents();

void logMousePositions();

// main
void finish();

void handleInput();

void render(SDL_Renderer *renderer, TTF_Font *fontNumbers, TTF_Font *fontFeedback);

void update(double deltaTime);

void renderText(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font);

int main(int argc, char **argv);

// Circle

void circleDistribution();

void renderFeedback();
void renderFeedbackText();

void createTargetArray();

// Strings

int renderStrings(void);

#endif
