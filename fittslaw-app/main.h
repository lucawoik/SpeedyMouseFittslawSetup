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
#include <linux/input.h>

#define WIDTH 1920
#define HEIGHT 1080


#define NUM_RADIUS 3
#define NUM_DISTANCE 3

/* #define ANGLE_NONE 5 */

/* number of circles in the circle */
#define NUM_CIRCLES 9

#define NUM_ITERATIONS_PER_ID  4

#define MAX_CLICKS 10000
#define MAX_EVENTS 100000

#define DEBUG 0

#define TARGET_COLOR 0xFF000000

#define TOTAL_NUM_CIRCLES NUM_CIRCLES * NUM_ITERATIONS_PER_ID

#define NUM_ITERATIONS NUM_RADIUS * NUM_DISTANCE * TOTAL_NUM_CIRCLES

/* TODO: hier könnte man noch nach static und mooving unterscheiden */
#define LOG_PATH "log"


#define centerX WIDTH/2
#define centerY HEIGHT/2 + 13

#define MAX_PATH_LENGTH 256

extern char EVENT_PATH[MAX_PATH_LENGTH];
extern int PARTICIPANT_ID;
extern int TRIAL;
extern int LEVEL_OF_LATENCY;
extern int IS_TEST_MODE;

extern int TARGET_RADIUS[NUM_RADIUS];
extern int TARGET_DISTANCE[NUM_DISTANCE];

extern int isSetupTarget;
extern int click_count_total;
extern void startEventLogging();
extern void *initEventLogging(void *arg);
extern void stopEventLogging();

/* TODO: nicht mehr notwendig, aber noch in den Logs drin */
/* static const char *ANGLE_STRING[] = {
    "towards", "towards_diagonal", "perpendicular", "away_diagonal", "away", "none",
}; */


typedef struct {
    double x;
    double y;
    int r;
    int d;
} Target;

/* TODO: Anpassen! */
/*     int a; */
/*     int v; */
typedef struct {
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
typedef struct {
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
    int radius;
    int distance;
} Tupel;


Target targetArray[NUM_ITERATIONS];

int fd;
int currently_logging;
int eventLogHeadWritten;

Trial trials[NUM_ITERATIONS];
Click clicks[MAX_CLICKS];
struct input_event events[MAX_EVENTS];
pthread_mutex_t eventArrayMutex;

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

// log
void logClicks();

// main
void finish();

void handleInput();

void render(SDL_Renderer* renderer, TTF_Font *fontNumbers, TTF_Font *fontFeedback);

void update(double deltaTime);

void renderText(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font);

int main(int argc, char** argv);


// Circle

void circleDistribution();

void renderFeedback();
void renderFeedbackText();

void createTargetArray();

// Strings

int renderStrings(void);

#endif
