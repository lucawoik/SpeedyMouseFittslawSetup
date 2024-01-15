#include "main.h"

void logClicks()
{
    //TODO: remove unwanted latency values, use only one...
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
    fprintf(logFile, "id,timestamp_ms,participant_id,trial,latency_click_min,latency_click_max,latency_move_min,latency_move_max,target_width,target_distance,target_x,target_y,cursor_x,cursor_y,distance_cursor_target,success\n");

    // print clicks
    /* clicks[i].v, */
    for (int i = 0; i < click_count_total; i++)
    {
        fprintf(logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                clicks[i].id,
                clicks[i].timestamp,
                PARTICIPANT_ID,
                EXPERIMENT,
                LATENCY_CLICK_MIN,
                LATENCY_CLICK_MAX,
                LATENCY_MOVE_MIN,
                LATENCY_MOVE_MAX,
                clicks[i].w,
                clicks[i].d,
                clicks[i].x_target,
                clicks[i].y_target,
                clicks[i].x_cursor,
                clicks[i].y_cursor,
                clicks[i].distance,
                clicks[i].success);

        /* printf( "%d %ld | %d %d | %d %d %d %d |%d %d | %d %d %d %d | %d %d\n",
                clicks[i].id,
                clicks[i].timestamp,

                PARTICIPANT_ID,
                EXPERIMENT,
                LATENCY_CLICK_MIN,
                LATENCY_CLICK_MAX,
                LATENCY_MOVE_MIN,
                LATENCY_MOVE_MAX,

                clicks[i].w,
                clicks[i].d,
                clicks[i].x_target,
                clicks[i].y_target,
                clicks[i].x_cursor,
                clicks[i].y_cursor,
                clicks[i].distance,
                clicks[i].success); */
    }

    if (fclose(logFile) == EOF)
    {
        printf("Error closing log file\n");
    }
}

void logTrials()
{
    //TODO: remove unused latency logs
    //TODO: see Notion
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
    fprintf(logFile, "id,timestamp_ms,participant_id,trial,latency_click_min,latency_click_max,latency_move_min,latency_move_max,target_width,target_distance,target_x,target_y,cursor_x,cursor_y,task_time_ms,clicks_needed,travel_distance,success\n");

    // print clicks
    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        fprintf(logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d,%d\n",
                trials[i].id,
                trials[i].timestamp,
                PARTICIPANT_ID,
                EXPERIMENT,
                LATENCY_CLICK_MIN,
                LATENCY_CLICK_MAX,
                LATENCY_MOVE_MIN,
                LATENCY_MOVE_MAX,
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

    // print Trials
    /* for(int i = 0; i < NUM_ITERATIONS; i++)
    {
       printf("%d %ld | %d %d | %d %d %d %d | %f %d %d %d\n",
               trials[i].id,
               trials[i].timestamp,
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
    } */

    if (fclose(logFile) == EOF)
    {
        printf("Error closing log file\n");
    }
}

void *initEventLogging(void *arg)
{
    // open the event handler
    fd = open(EVENT_PATH, O_RDONLY); // | NONBLOCk in oder to make the read non-blocking
    if (fd == -1)
    {
        perror("Error opening evdev device");
        return NULL;
    }

    printf("Event Logging Initialized\n");

    while (1)
    {
        while (currently_logging == 1)
        {
            // Reading the current event-struct and saving it to the events array
            ssize_t bytesRead = read(fd, &events[eventCount], sizeof(struct input_event));

            if (bytesRead == -1)
            {
                perror("Error reading from evdev device");
                close(fd);
                return NULL;
            }

            // Counting events to keep track of the number of events already saved
            if (bytesRead == sizeof(struct input_event))
            {
                eventCount++;
            }
        }
    }
}

void startEventLogging()
{
       currently_logging = 1;
       printf("Logging started\n");
}

void stopEventLogging()
{
    currently_logging = 0;
    // creating file to save logs to
    char path[256];
    sprintf(path, "%s/mouse_events_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, EXPERIMENT);

    struct stat st_directory = {0};

    // create log directory if it doesn't exist
    if (stat(LOG_PATH, &st_directory) == -1)
    {
        mkdir(LOG_PATH, 0777);
    }

    // creating or appending to the log file
    FILE *logFile = fopen(path, "a");

    if (logFile == NULL)
    {
        printf("Error opening log file\n");
        return;
    }

    // Write table head for each round
    // TODO: replace with zeroes
    fprintf(logFile, "tv_sec, tv_usec, type, code, value\n");

    for (int i = 0; i < eventCount; i++)
    {
        fprintf(logFile, "%ld,%ld,%u,%u,%d\n",
                events[i].time.tv_sec,
                events[i].time.tv_usec,
                events[i].type,
                events[i].code,
                events[i].value);
    }

    fclose(logFile);
    printf("Mouse events saved to:\n%s/mouse_events_participant_%d_trial_%d.csv\n", LOG_PATH, PARTICIPANT_ID, EXPERIMENT);

    // Reset for the next logging interval
    eventCount = 0;
}
