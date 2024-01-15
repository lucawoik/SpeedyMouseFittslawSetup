#include "main.h"

void logClicks()
{
    char path[256];
    sprintf(path, "%s/static_clicks_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, EXPERIMENT);

    struct stat st_directory = {0};

    // create log directory if it doesn't exist
    if (stat(LOG_PATH, &st_directory) == -1)
    {
        mkdir(LOG_PATH, 0777);
    }

    FILE *logFile = fopen(path, "w");

    if (logFile == NULL)
    {
        printf("Error opening log file\n");
        return;
    }
    /* TODO: schaun ob man immmer noch alles braucht (nachdem man nur noch ein klich hat pro ziel)
     */
    fprintf(logFile, "id,timestamp_ms,participant_id,trial,latency,target_width,target_distance,target_x,target_y,cursor_x,cursor_y,distance_cursor_target,success\n");

    // print clicks
    /* clicks[i].v, */
    for (int i = 0; i < click_count_total; i++)
    {
        fprintf(logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                clicks[i].id,
                clicks[i].timestamp,
                PARTICIPANT_ID,
                EXPERIMENT,
                LATENCY,
                clicks[i].w,
                clicks[i].d,
                clicks[i].x_target,
                clicks[i].y_target,
                clicks[i].x_cursor,
                clicks[i].y_cursor,
                clicks[i].distance,
                clicks[i].success);
    }

    if (fclose(logFile) == EOF)
    {
        printf("Error closing log file\n");
    }
}

void logTrials()
{
    char path[256];
    sprintf(path, "%s/static_iterations_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, EXPERIMENT);

    struct stat st_directory = {0};

    // create log directory if it doesn't exist
    if (stat(LOG_PATH, &st_directory) == -1)
        mkdir(LOG_PATH, 0777);

    FILE *logFile = fopen(path, "w");

    if (logFile == NULL)
    {
        printf("Error opening log file\n");
        return;
    }
    /* TODO: schaun ob man immmer noch alles braucht (nachdem man nur noch ein klich hat pro ziel)
     */
    fprintf(logFile, "id,timestamp_ms,participant_id,trial,latency,target_width,target_distance,target_x,target_y,cursor_x,cursor_y,task_time_ms,clicks_needed,travel_distance,success\n");

    // print clicks
    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        fprintf(logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d,%d\n",
                trials[i].id,
                trials[i].timestamp,
                PARTICIPANT_ID,
                EXPERIMENT,
                LATENCY,
                trials[i].w,
                trials[i].d,

                trials[i].x_target,
                trials[i].y_target,
                trials[i].x_cursor,
                trials[i].y_cursor,
                trials[i].time,
                trials[i].clicks,
                trials[i].travel_distance,
                trials[i].success);
    }

    if (fclose(logFile) == EOF)
    {
        printf("Error closing log file\n");
    }
}
