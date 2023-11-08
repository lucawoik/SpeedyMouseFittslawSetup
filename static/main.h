#ifndef __fitts_main__
#define __fitts_main__

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>

#define WIDTH 1920
#define HEIGHT 1080

/* #define EASY 0
#define MEDIUM 1
#define HARD 2 */

#define NUM_RADIUS 4
/* todo: nicht mehr notwenid */
/* #define NUM_DISTANCE 1
#define NUM_VELOCITY 2 */
/* #define NUM_ANGLE 5 */

/* #define ANGLE_TOWARDS 0
#define ANGLE_DIAGONAL_TOWARDS 1
#define ANGLE_PERPENDICULAR 2
#define ANGLE_DIAGONAL_AWAY 3
#define ANGLE_AWAY 4 */
#define ANGLE_NONE 5

/* number of circles in the circle */
#define NUM_CIRCLES 7

#define MAX_CLICKS 10000

#define DEBUG 0

#define TARGET_COLOR 0xFF000000
/* todo: NUM_DISTANCE nicht mehr notwenid */
/* * NUM_DISTANCE * NUM_VELOCITY  */
/* TODO: neu wäre: Num_Radius * NUM_distances * NUM_CIRCLES */
#define NUM_ITERATIONS NUM_RADIUS

#define LOG_PATH "log"

extern int PARTICIPANT_ID;
extern int EXPERIMENT;
extern int LATENCY_CLICK_MIN;
extern int LATENCY_CLICK_MAX;
extern int LATENCY_MOVE_MIN;
extern int LATENCY_MOVE_MAX;

extern int TARGET_RADIUS[NUM_RADIUS];
// TODO: brauch ich später nicht mehr (ziel wird ja anders positioniert)
/* extern int TARGET_DISTANCE[NUM_DISTANCE];
extern int TARGET_VELOCITY[NUM_VELOCITY]; */
// extern int TARGET_ANGLE[NUM_ANGLE];

extern int isSetupTarget;
extern int click_count_total;

/* nicht mehr notwendig, aber noch in den Logs drin */
static const char *ANGLE_STRING[] = {
    "towards", "towards_diagonal", "perpendicular", "away_diagonal", "away", "none",
};
/*     int d;
    int v;
    int a; */
typedef struct {
    int r;

} TargetTemplate;

/*     int d;
    int v;
    int a; */
    /*     double vX;
    double vY; */

typedef struct {
    double x;
    double y;
    int r;

} Target;

/* Anpassen! */
typedef struct {
    int id;
    long timestamp;
    int w;
    int d;
    int v;
    int a;
    int x_target;
    int y_target;
    int x_cursor;
    int y_cursor;
    double time; // in ms
    int clicks;
    int travel_distance;
    int success;
} Trial;

typedef struct {
    int id;
    long timestamp;
    int w;
    int d;
    int v;
    int a;
    int x_target;
    int y_target;
    int x_cursor;
    int y_cursor;
    int distance; // target -> cursor
    int success;
} Click;

TargetTemplate staticTargetTemplates[NUM_ITERATIONS];
/* TargetTemplate movingTargetTemplates[NUM_ITERATIONS / 2]; */
TargetTemplate *targetTemplates[NUM_ITERATIONS];

Trial trials[NUM_ITERATIONS];
Click clicks[MAX_CLICKS];

// templates
/* , int d, int v, int a */
/* TargetTemplate createTargetTemplate(int r); */

/* x und y waren bisher immer Mous x und y; 
aber für ein Target braucht man die ja jetzt eigenltich nicht mehr */
/* man könnte jetzt eigentlich create createTargetTemplate und create target zusammen ziehen
weil das erste setzt ja nur den radius  um*/
/* die radiie wären dann immer in der gleichen reihenfolge!!! */
/* Target createTarget(int x, int y, TargetTemplate *targetTemplate); */
Target createTarget(int x, int y, int r);

void initTargetTemplates();

// utils
void swap(TargetTemplate *a, TargetTemplate *b);

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

void render(SDL_Renderer* renderer);

void update(double deltaTime);

int main(int argc, char** argv);

// Circle

void circleDistribution();

#endif
