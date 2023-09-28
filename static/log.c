#include "main.h"

// TODO

void logClicks()
{
    char path[256];
    sprintf(path, "%s/clicks_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, EXPERIMENT);

	struct stat st_directory = {0};

    // create log directory if it doesn't exist
	if (stat(LOG_PATH, &st_directory) == -1) mkdir(LOG_PATH, 0777);

	FILE *logFile = fopen(path, "w");

	if (logFile == NULL)
	{
		printf("Error opening log file\n");
		return;
	}

    fprintf(logFile, "id,timestamp_ms,participant_id,trial,latency_click_min,latency_click_max,latency_move_min,latency_move_max,target_width,target_distance,target_velocity,target_direction,target_x,target_y,cursor_x,cursor_y,distance_cursor_target,success\n");

    // print clicks
    for(int i = 0; i < click_count_total; i++)
    {
        fprintf( logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d\n", 
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
                clicks[i].v,
                ANGLE_STRING[clicks[i].a],
                clicks[i].x_target,
                clicks[i].y_target,
                clicks[i].x_cursor,
                clicks[i].y_cursor,
                clicks[i].distance,
                clicks[i].success);

        /*
        printf( "%d %ld | %d %d %d %d | %d %d %d %d | %d %d\n", 
                clicks[i].id,
                clicks[i].timestamp,
                clicks[i].w,
                clicks[i].d,
                clicks[i].v,
                clicks[i].a,
                clicks[i].x_target,
                clicks[i].y_target,
                clicks[i].x_cursor,
                clicks[i].y_cursor,
                clicks[i].distance,
                clicks[i].success);
        */
    }

	if (fclose(logFile) == EOF)
	{
		printf("Error closing log file\n");
	}
}

void logTrials()
{
    char path[256];
    sprintf(path, "%s/iterations_participant_%d_trial_%d.csv", LOG_PATH, PARTICIPANT_ID, EXPERIMENT);

	struct stat st_directory = {0};

    // create log directory if it doesn't exist
	if (stat(LOG_PATH, &st_directory) == -1) mkdir(LOG_PATH, 0777);

	FILE *logFile = fopen(path, "w");

	if (logFile == NULL)
	{
		printf("Error opening log file\n");
		return;
	}

    fprintf(logFile, "id,timestamp_ms,participant_id,trial,latency_click_min,latency_click_max,latency_move_min,latency_move_max,target_width,target_distance,target_velocity,target_direction,target_x,target_y,cursor_x,cursor_y,task_time_ms,clicks_needed,travel_distance,success\n");

    // print clicks
    for(int i = 0; i < NUM_ITERATIONS; i++)
    {
        fprintf( logFile,
                "%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%d,%d,%d,%d,%f,%d,%d,%d\n", 
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
                trials[i].v,
                ANGLE_STRING[trials[i].a],
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
    //for(int i = 0; i < NUM_ITERATIONS; i++)
    //{
    //    printf( "%d %ld | %d %d %d %d | %d %d %d %d | %f %d %d %d\n",
    //            trials[i].id,
    //            trials[i].timestamp,
    //            trials[i].w,
    //            trials[i].d,
    //            trials[i].v,
    //            trials[i].a,
    //            trials[i].x_target,
    //            trials[i].y_target,
    //            trials[i].x_cursor,
    //            trials[i].y_cursor,
    //            trials[i].time,
    //            trials[i].clicks,
    //            trials[i].travel_distance,
    //            trials[i].success);
    //}

	if (fclose(logFile) == EOF)
	{
		printf("Error closing log file\n");
	}
}
