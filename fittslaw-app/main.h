#ifndef __fitts_main__
#define __fitts_main__

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

#define WIDTH 1920
#define HEIGHT 1080


#define NUM_RADIUS 3
#define NUM_DISTANCE 3

/* #define ANGLE_NONE 5 */

/* number of circles in the circle */
#define NUM_CIRCLES 9

#define MAX_CLICKS 10000

#define DEBUG 0

#define TARGET_COLOR 0xFF000000

#define NUM_ITERATIONS NUM_RADIUS * NUM_DISTANCE * NUM_CIRCLES

/* TODO: hier könnte man noch nach static und mooving unterscheiden */
#define LOG_PATH "log"


#define centerX WIDTH/2
#define centerY HEIGHT/2


extern int PARTICIPANT_ID;
extern int EXPERIMENT;
extern int LATENCY_CLICK_MIN;
extern int LATENCY_CLICK_MAX;
extern int LATENCY_MOVE_MIN;
extern int LATENCY_MOVE_MAX;

extern int TARGET_RADIUS[NUM_RADIUS];
extern int TARGET_DISTANCE[NUM_DISTANCE];

extern int isSetupTarget;
extern int click_count_total;

/* TODO: nicht mehr notwendig, aber noch in den Logs drin */
/* static const char *ANGLE_STRING[] = {
    "towards", "towards_diagonal", "perpendicular", "away_diagonal", "away", "none",
}; */


typedef struct {
    double x;
    double y;
    int r;
    int d;
    bool success;
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
    int distance; // target -> cursor
    int success;
} Click;


typedef struct 
{
    int radius;
    int distance;
} Tupel;


Target targetArray[NUM_ITERATIONS];


Trial trials[NUM_ITERATIONS];
Click clicks[MAX_CLICKS];

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

// log
void logClicks();

void logTrials();

// main
void finish();

void handleInput();

void render(SDL_Renderer* renderer, TTF_Font *font);

void update(double deltaTime);

void render_numbers(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font);

int main(int argc, char** argv);


// Circle

void circleDistribution();

void createTargetArray();

// Strings

int renderStrings(void);

#endif
