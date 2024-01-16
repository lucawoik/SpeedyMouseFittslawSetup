#include "main.h"

// Function to log clicks during rounds
void logClicks()
{
    // Constructing the path for the log file based on participant ID and trial condition
    char path[256];
    sprintf(path, "%s/clicks_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, CONDITION);

    struct stat st_directory = {0};

    // Checking if the log directory exists, and creating it if not
    if (stat(LOG_PATH, &st_directory) == -1)
    {
        mkdir(LOG_PATH, 0777);
    }

    // Opening the log file for writing and checking if the file opening was successful
    FILE *logFile = fopen(path, "w");

    if (logFile == NULL)
    {
        printf("Error opening log file\n");
        return;
    }

    // Writing the header of the CSV file
    fprintf(logFile, "id,timestamp_ms,participant_id,condition,level_of_latency,target_number,target_width,target_amplitude,target_x,target_y,cursor_x,cursor_y,success,completion_time\n");

    // Iterating through each recorded click and writing data to the log file
    for (int i = 0; i < click_count_total; i++)
    {
        fprintf(logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f\n",
                clicks[i].id,
                clicks[i].timestamp,
                PARTICIPANT_ID,
                CONDITION,
                LEVEL_OF_LATENCY,
                clicks[i].id%9,
                clicks[i].w,
                clicks[i].d,
                clicks[i].x_target,
                clicks[i].y_target,
                clicks[i].x_cursor,
                clicks[i].y_cursor,
                clicks[i].success,
                clicks[i].completion_time);
    }

    // Closing the log file and checking for errors
    if (fclose(logFile) == EOF)
    {
        printf("Error closing log file\n");
    }
}

void *initEventLogging(void *arg)
{
    // open the event handler
    fd = open(EVENT_PATH, O_RDONLY); // | NONBLOCK in oder to make the read non-blocking
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
    sprintf(path, "%s/mouse_events_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, CONDITION);

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
    // TODO: replace the zeroes
    fprintf(logFile, "participant_id, condition, level_of_latency, target_width, target_amplitude, tv_sec, tv_usec, type, code, value\n");

    for (int i = 0; i < eventCount; i++)
    {
        fprintf(logFile, "%d,%d,%d,%d,%d,%ld,%ld,%u,%u,%d\n",
                PARTICIPANT_ID,
                CONDITION,
                LEVEL_OF_LATENCY,
                0,
                0,
                events[i].time.tv_sec,
                events[i].time.tv_usec,
                events[i].type,
                events[i].code,
                events[i].value);
    }

    fclose(logFile);
    printf("Mouse events saved to:\n%s/mouse_events_participant_%d_trial_%d.csv\n", LOG_PATH, PARTICIPANT_ID, CONDITION);

    // Reset for the next logging interval
    eventCount = 0;
}
