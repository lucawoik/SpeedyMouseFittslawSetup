#include "main.h"

int logsInitialized = 0;
int actualHeadWritten = 0;

char eventsPath[256];
int roundCounter;

long currentInterval = 0;

// ------------------------------
//
// Clicks
//
// ------------------------------

/* Logging all clicks during rounds */
void logClicks()
{
    // Constructing the path for the log file based on participant ID and trial
    char path[256];
    sprintf(path, "%s/clicks_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, TRIAL);

    struct stat st_directory = {0};

    // Checking if the log directory exists, and creating it if not
    if (stat(LOG_PATH, &st_directory) == -1)
    {
        mkdir(LOG_PATH, 0777);
    }

    // Opening the log file for writing and checking if the file opening was successful
    FILE *logFile = fopen(path, "a");

    if (logFile == NULL)
    {
        printf("Error opening log file\n");
        return;
    }

    // Writing the header of the CSV file
    fprintf(logFile, "id,timestamp_ms,participant_id,trial,level_of_latency,target_number,target_width,target_amplitude,target_x,target_y,cursor_x,cursor_y,success,completion_time\n");

    // Iterating through each recorded click and writing data to the log file
    for (int i = 0; i < click_count_total; i++)
    {
        fprintf(logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f\n",
                clicks[i].id,
                clicks[i].timestamp,
                PARTICIPANT_ID,
                TRIAL,
                LEVEL_OF_LATENCY,
                clicks[i].id % 9,
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

// ------------------------------
//
// Events
//
// ------------------------------

/* Append interval and predicted events to logging arrays */
void appendEvents(int intervalX, int intervalY, float predX, float predY)
{
    if (isLogging)
    {
        if (currentInterval < MAX_EVENTS)
        {
            long currentTimestamp = millis();
            // lock the arrays to prevent race conditions (although unlikely)
            pthread_mutex_lock(&loggedEventsMutex);

            // Save the values as LogEvent
            LogEvent currentEvents;
            currentEvents.timestamp = currentTimestamp;
            currentEvents.intervalX = intervalX;
            currentEvents.intervalY = intervalY;
            currentEvents.predictedX = predX;
            currentEvents.predictedY = predY;

            loggedEvents[currentInterval] = currentEvents;

            // unlock
            pthread_mutex_unlock(&loggedEventsMutex);

            currentInterval++;
        }
        else
        {
            perror("Interval and prediction logging array is full");
            exit(1);
        }
    }
}

/* write the logged events to .csv file */
void logEvents()
{
    // create files if logEvents() is called for the first time
    if (logsInitialized == 0)
    {
        sprintf(eventsPath, "%s/mouse_events_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, TRIAL);

        struct stat st_directory = {0};

        // create log directory if it doesn't exist
        if (stat(LOG_PATH, &st_directory) == -1)
        {
            mkdir(LOG_PATH, 0777);
        }
    }

    // creating the log file or appending
    FILE *logFile = fopen(eventsPath, "a");

    if (logFile == NULL)
    {
        printf("Error opening log file\n");
        return;
    }

    // Write table head for the first round
    if (logsInitialized == 0)
    {
        fprintf(logFile, "participant_id,trial,round,level_of_latency,timestamp_ms,interval_x,interval_y,predicted_x,predicted_y\n");
    }

    pthread_mutex_lock(&loggedEventsMutex);
    for (int i = 0; i < currentInterval; i++)
    {
        fprintf(logFile, "%d,%d,%d,%d,%ld,%d,%d,%f,%f\n",
                PARTICIPANT_ID,
                TRIAL,
                roundCounter,
                LEVEL_OF_LATENCY,
                loggedEvents[i].timestamp,
                loggedEvents[i].intervalX,
                loggedEvents[i].intervalY,
                loggedEvents[i].predictedX,
                loggedEvents[i].predictedX);
    }
    pthread_mutex_unlock(&loggedEventsMutex);

    if (fclose(logFile) == EOF)
    {
        printf("Error closing log file\n");
    }

    logsInitialized = 1;
    roundCounter++;
}

// ------------------------------
//
// Mouse positions
//
// ------------------------------

/* Function writing all mouse positions to CSV  */

void logMousePositions()
{
    // Constructing the path for the log file based on participant ID and trial
    char path[256];
    sprintf(path, "%s/mouse_positions_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, TRIAL);

    struct stat st_directory = {0};

    // Checking if the log directory exists, and creating it if not
    if (stat(LOG_PATH, &st_directory) == -1)
    {
        mkdir(LOG_PATH, 0777);
    }

    // Opening the log file for writing and checking if the file opening was successful
    FILE *logFile = fopen(path, "a");

    if (logFile == NULL)
    {
        printf("Error opening log file\n");
        return;
    }

    // Writing the header of the CSV file
    fprintf(logFile, "id,participant_id,trial,level_of_latency,timestamp_ms,abs_x,abs_y\n");

    // Iterating through each recorded click and writing data to the log file
    for (int i = 0; i < positionsLogged; i++)
    {
        fprintf(logFile,
                "%d,%d,%d,%d,%ld,%d,%d\n",
                positions[i].id,
                PARTICIPANT_ID,
                TRIAL,
                LEVEL_OF_LATENCY,
                positions[i].timestamp,
                positions[i].x,
                positions[i].y);
    }

    // Closing the log file and checking for errors
    if (fclose(logFile) == EOF)
    {
        printf("Error closing log file\n");
    }
}