#include "main.h"

// Input devices
struct libevdev *event_dev = NULL;
struct libevdev *uinput_dev = NULL;

long intervalStart = 0;
long intervalStop = 0;

// total input values for each interval
int intervalX = 0;
int intervalY = 0;

CircularBuffer eventsBuffer;

// ANN stuff
TF_Graph *Graph;
TF_Status *Status;

// use attributes to create threads in a detached state
// source: https://github.com/PDA-UR/DelayDaemon
pthread_attr_t invoked_event_thread_attr;

// ------------------------------
//
// Helper functions
//
// ------------------------------

int compare(const void *a, const void *b)
{
	float fa = *((float *)a);
	float fb = *((float *)b);
	return (fa > fb) - (fa < fb);
}

float calculateMedian(float arr[], int size)
{
	qsort(arr, size, sizeof(float), compare);

	return arr[size/2];
}

/**
 * Circular Buffer
 *  - addEvent()
 *  - initCircularBuffer()
 */

void addEvent(CircularBuffer *buffer, NormalizedEvent event)
{
    buffer->front = (buffer->front + 1) % BUFFER_LENGTH;
    buffer->events[buffer->front] = event;
}

void initCircularBuffer(CircularBuffer *buffer)
{
    /* init buffer and fill with all zeroes */
    buffer->front = -1;

    NormalizedEvent eventZero;
    eventZero.x = normalize(0.0f, 'x');
    eventZero.y = normalize(0.0f, 'y');

    // init buffer with all zeros to be able to call the ann right from the beginning
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        addEvent(buffer, eventZero);
    }
}

/**
 * Inputs and Events
 *  - emit()
 *      - creates an input event for the specified device
 *      - source: https://www.kernel.org/doc/html/v4.12/input/uinput.html
 *  - emitKlick()
 *  - emitRel()
 *  - getEvent()
 *      - get the next input event from libevdev
 *      - source: https://github.com/thomfischer/DelayDaemon/tree/master
 *  - init_input_device()
 *      - source: https://github.com/thomfischer/DelayDaemon/tree/master
        - open the input device that should be predicted
    - init_virtual_input()
        - create a virtual input device
        - this device is used to trigger delayed input events
        - source: https://www.freedesktop.org/software/libevdev/doc/latest/group__uinput.html#gaf14b21301bac9d79c20e890172873b96
 */

void emit(int type, int code, int val)
{
    int rc = libevdev_uinput_write_event(
        uinput_dev, type,
        code, val);

    if (rc != 0)
        printf("Failed to write uinput event: %s\n", strerror(-rc));

    rc = libevdev_uinput_write_event(uinput_dev, EV_SYN, SYN_REPORT, 0);
}

void emitKlick(int value)
{
    emit(EV_MSC, MSC_SCAN, 9001);
    emit(EV_KEY, BTN_LEFT, value);
    emit(EV_SYN, SYN_REPORT, 0);
}

void *emitRel(void *args)
{
    // Source: https://github.com/PDA-UR/DelayDaemon
    DelayedEvent *event = args;
    int x = event->x;
    int y = event->y;
    int delay_ms = event->delay_ms;
    free(event);

    usleep(delay_ms * 1000);

    emit(EV_REL, REL_X, x);
    emit(EV_REL, REL_Y, y);
    emit(EV_SYN, SYN_REPORT, 0); // Syn-event

    pthread_exit(NULL);
}

int getEvent(struct input_event *event)
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    int rc = LIBEVDEV_READ_STATUS_SUCCESS;

    rc = libevdev_next_event(event_dev,
                             LIBEVDEV_READ_FLAG_NORMAL |
                                 LIBEVDEV_READ_FLAG_BLOCKING,
                             event);

    /* Handle dropped SYN. */
    if (rc == LIBEVDEV_READ_STATUS_SYNC)
    {
        printf("Warning, syn dropped: (%d) %s\n", -rc, strerror(-rc));

        while (rc == LIBEVDEV_READ_STATUS_SYNC)
        {
            rc = libevdev_next_event(event_dev,
                                     LIBEVDEV_READ_FLAG_SYNC, event);
        }
    }

    if (rc == -ENODEV)
    {
        printf("Device disconnected: (%d) %s\n", -rc, strerror(-rc));
        return -1;
    }
    return 1;
}

int init_input_device()
{
    // Open device.
    int fd_event = open(EVENT_PATH, O_RDONLY | O_NONBLOCK);
    if (fd_event < 0)
    {
        perror("Failed to open input device");
        if (!IS_TEST_MODE)
            exit(EXIT_FAILURE);
        return 0;
    }

    // Create libevdev device and grab it.
    if (libevdev_new_from_fd(fd_event, &event_dev) < 0)
    {
        perror("Failed to init libevdev");
        exit(EXIT_FAILURE);
    }

    if (libevdev_grab(event_dev, LIBEVDEV_GRAB) < 0)
    {
        perror("Failed to grab device");
        exit(EXIT_FAILURE);
    }

    return 1;
}

int init_virtual_input()
{
    // Create uinput clone of device.
    int fd_uinput = open("/dev/uinput", O_WRONLY);
    if (fd_uinput < 0)
    {
        perror("Failed to open uinput device");
        exit(EXIT_FAILURE);
    }

    if (libevdev_uinput_create_from_device(event_dev, fd_uinput, &uinput_dev) < 0)
    {
        perror("Failed to create uinput device");
        exit(EXIT_FAILURE);
    }

    return 1;
}

/*
 * Tensorflow C API helpers
 * Source: https://github.com/AmirulOm/tensorflow_capi_sample/tree/master?tab=readme-ov-file
 *  - NoOpDeallocator()
 *  - createSession()
 *  - getOutput()
 *  - getTensor()
 *  - getTensor()
 */

// Helper function for accessing Tensorflow C API
void NoOpDeallocator(void *data, size_t a, void *b) {}

TF_Session *createSession()
{
    // Read model
    Graph = TF_NewGraph();
    Status = TF_NewStatus();

    TF_SessionOptions *SessionOpts = TF_NewSessionOptions();
    TF_Buffer *RunOpts = NULL;

    const char *tags = "serve"; // default model serving tag
    int ntags = 1;

    TF_Session *Session = TF_LoadSessionFromSavedModel(SessionOpts, RunOpts, settings[currentSetting].model_path, &tags, ntags, Graph, NULL, Status);
    if (TF_GetCode(Status) != TF_OK)
    {
        printf("%s", TF_Message(Status));
    }

    return Session;
}

TF_Output getOutput(char *name, int number)
{
    TF_Output t = {TF_GraphOperationByName(Graph, name), number};
    if (t.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName %s\n", name);

    return t;
}

TF_Tensor *getTensor(int ndims, int64_t dims[], float data[], int ndata)
{
    TF_Tensor *tensor = TF_NewTensor(TF_FLOAT, dims, ndims, data, ndata, &NoOpDeallocator, 0);
    if (tensor == NULL)
    {
        printf("ERROR: Failed TF_NewTensor\n");
    }

    return tensor;
}

float getOutputValues(int index, TF_Tensor **OutputValues)
{
    void *buff = TF_TensorData(OutputValues[index]);
    float *offsets = buff;

    return offsets[0];
}

// ------------------------------
//
// "Main"-Fuction
//
// ------------------------------

/*
 * manipulateMouseEvents()
 *  - Resample the input, run the ANN and emit the predicted events
 */

void *manipulateMouseEvents(void *arg)
{
    /* Initializing model with Tensorflow C API */
    TF_Session *Session = createSession();

    // Get input tensor
    int NumInputs = 2;
    TF_Output *Input = malloc(sizeof(TF_Output) * NumInputs);

    Input[0] = getOutput("serving_default_last_x_values", 0);
    Input[1] = getOutput("serving_default_last_y_values", 0);

    // Get Output tensor
    int NumOutputs = 2;
    TF_Output *Output = malloc(sizeof(TF_Output) * NumOutputs);

    Output[0] = getOutput("StatefulPartitionedCall", 0);
    Output[1] = getOutput("StatefulPartitionedCall", 1);

    // Allocate data for inputs & outputs
    TF_Tensor **InputValues = (TF_Tensor **)malloc(sizeof(TF_Tensor *) * NumInputs);
    TF_Tensor **OutputValues = malloc(sizeof(TF_Tensor *) * NumOutputs);

    /* Init input devices, resampled buffer, get event & current event variable */
    if (!init_input_device())
        return NULL;
    if (!init_virtual_input())
        return NULL;

    initCircularBuffer(&eventsBuffer);

    int err = -1;
    struct input_event currentEvent;

    NormalizedEvent normalizedEvent;

    int ndims = 2;
    int64_t dims[] = {1, BUFFER_LENGTH};
    float dataX[BUFFER_LENGTH]; // these two arrays store the history of resampled events for x and y respectively
    float dataY[BUFFER_LENGTH];
    int ndata = BUFFER_LENGTH * sizeof(float); // This is tricky, it number of bytes not number of element

    /* TODO: better inference-time measurement */
    long long totalTime = 0;
    long intervals = 0;

    /* Entering interval-loop */
    while (true)
    {
        /* Defining the currents interval start and end */
        intervalStart = millis();
        intervalStop = intervalStart + INTERVAL_LENGTH;

        while (true)
        {
            if (millis() > intervalStop) // breaking if intervall is over
            {
                break;
            }

            err = getEvent(&currentEvent); // read all events during the interval

            // classify event and react accordingly
            if (err > -1 && currentEvent.type != EV_SYN && currentEvent.type != EV_MSC)
            {
                // Add current event to sum
                if (currentEvent.type == EV_REL)
                {
                    if (currentEvent.code == REL_X)
                    {
                        intervalX += currentEvent.value;
                    }
                    if (currentEvent.code == REL_Y)
                    {
                        intervalY += currentEvent.value;
                    }
                }
                else if (currentEvent.type == EV_KEY) // Register klicks right away
                {
                    emitKlick(currentEvent.value);
                }
            }
        }

        // TODO: finalize and log time measurement
        long long start = micros();

        /* normalize the resampled event and add to buffer */
        normalizedEvent.x = normalize((float)intervalX, 'x');
        normalizedEvent.y = normalize((float)intervalY, 'y');
        addEvent(&eventsBuffer, normalizedEvent);

        /* get prediction from buffer */
        // filling datax and y in the correct sequence
        for (int i = BUFFER_LENGTH - 1; i >= 0; i--)
        {
            int index = (eventsBuffer.front - i + BUFFER_LENGTH - 1) % BUFFER_LENGTH;
            dataX[i] = eventsBuffer.events[index].x;
            dataY[i] = eventsBuffer.events[index].y;
        }

        // Create input tensors
        InputValues[0] = getTensor(ndims, dims, dataX, ndata);
        InputValues[1] = getTensor(ndims, dims, dataY, ndata);

        // Run the Session
        TF_SessionRun(Session, NULL, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, NULL, 0, NULL, Status);
        if (TF_GetCode(Status) != TF_OK)
        {
            printf("%s", TF_Message(Status));
        }

        // get and de-normalize the output values
        float predX = denormalize(getOutputValues(0, OutputValues), 'x');
        float predY = denormalize(getOutputValues(1, OutputValues), 'y');

        /* process predictions */
        DelayedEvent *event = malloc(sizeof(DelayedEvent));
        if (settings[currentSetting].prediction_active)
        {
            event->x = (int)roundf(predX) + intervalX;
            event->y = (int)roundf(predY) + intervalY;
        }
        else
        {
            event->x = intervalX;
            event->y = intervalY;
        }
        event->delay_ms = settings[currentSetting].delay_ms;

        // emit predicted events
        pthread_t delayed_event_thread;
        pthread_create(&delayed_event_thread, &invoked_event_thread_attr, emitRel, event);
        printf("Emitted events: x %d, y %d\n", event->x, event->y);

        // Write to logging array
        appendEvents(intervalX, intervalY, settings[currentSetting].prediction_active ? predX : 0.0f, settings[currentSetting].prediction_active ? predY : 0.0f);

        // TODO: for debugging
        printf("Resampled events:   x->%d, y->%d\n", intervalX, intervalY);
        totalTime += micros() - start;
        intervals++;
        printf("Inference time %lld\n", totalTime / intervals);

        /* reset for next interval */
        intervalX = 0;
        intervalY = 0;
    }
    /* onto the next interval... */
}
