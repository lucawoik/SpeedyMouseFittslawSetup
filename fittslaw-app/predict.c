#include "main.h"

// Input devices
struct libevdev *event_dev = NULL;
struct libevdev *uinput_dev = NULL;

long intervalStart = 0;
long intervalStop = 0;

// total input values for each interval
int intervalX = 0;
int intervalY = 0;

CircularBuffer resampledEventsBuffer;

// ANN stuff
TF_Graph *Graph;
TF_Status *Status;

//****** Helper functions
// Helpers for circular Buffer
void addEvent(CircularBuffer *buffer, ResampledEvent event)
{
    buffer->front = (buffer->front + 1) % BUFFER_LENGTH;
    buffer->events[buffer->front] = event;
}

void initCircularBuffer(CircularBuffer *buffer)
{
    /* init buffer and fill with all zeroes */
    buffer->front = -1;

    ResampledEvent eventZero;
    eventZero.x = 0.43065032f;
    eventZero.y = 0.47260916f;

    // init buffer with all zeros to be able to call the ann right from the beginning
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        addEvent(buffer, eventZero);
        // printf("Filling buffer -> %d\n", i);
    }
}

// Helpers for input and events
// creates an input event for the specified device
// source: https://www.kernel.org/doc/html/v4.12/input/uinput.html
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

void emitRel(int x, int y)
{
    emit(EV_REL, REL_X, x);
    emit(EV_REL, REL_Y, y);
    emit(EV_SYN, SYN_REPORT, 0); // Syn-event
    // TODO: Print for debugging purposes
    printf("Emitted events:     x->%d, y->%d\n", x, y);
}

// get the next input event from libevdev
// source: https://github.com/thomfischer/DelayDaemon/tree/master
int get_event(struct input_event *event)
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

// Input helpers, source: https://github.com/thomfischer/DelayDaemon/tree/master
// open the input device that should be predicted
int init_input_device()
{
    /* Open device. */
    int fd_event = open(EVENT_PATH, O_RDONLY | O_NONBLOCK);
    if (fd_event < 0)
    {
        perror("Failed to open input device");
        if (!IS_TEST_MODE)
            exit(EXIT_FAILURE);
        return 0;
    }

    /* Create libevdev device and grab it. */
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

// create a virtual input device
// this device is used to trigger delayed input events
// source: https://www.freedesktop.org/software/libevdev/doc/latest/group__uinput.html#gaf14b21301bac9d79c20e890172873b96
int init_virtual_input()
{
    /* Create uinput clone of device. */
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

// ANN helper functions
// Source: https://github.com/AmirulOm/tensorflow_capi_sample/tree/master?tab=readme-ov-file

// Helper function for accessing Tensorflow C API
void NoOpDeallocator(void *data, size_t a, void *b) {}

TF_Session *createSession()
{
    // Read model
    Graph = TF_NewGraph();
    Status = TF_NewStatus();

    TF_SessionOptions *SessionOpts = TF_NewSessionOptions();
    TF_Buffer *RunOpts = NULL;

    const char *saved_model_dir = SAVED_MODEL_DIR; // Path of the model
    const char *tags = "serve";                    // default model serving tag
    int ntags = 1;

    TF_Session *Session = TF_LoadSessionFromSavedModel(SessionOpts, RunOpts, saved_model_dir, &tags, ntags, Graph, NULL, Status);
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

//******* MAIN
// Resample the input, run the ANN and emit the predicted events
void *manipulateMouseEvents(void *arg)
{
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

    // Init inputs and resampled buffer
    if (!init_input_device())
        return NULL;
    if (!init_virtual_input())
        return NULL;

    initCircularBuffer(&resampledEventsBuffer);

    /*
    // print buffer contents for debugging
    printf("Circular Buffer contents:\n");
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        int index = (resampledEventsBuffer.front - i + BUFFER_LENGTH) % BUFFER_LENGTH;
        printf("%d  x: %f, y: %f\n", i, resampledEventsBuffer.events[index].x, resampledEventsBuffer.events[index].y);
    }
     */

    // TODO: better inferenc-time measurement
    long long totalTime = 0;
    long intervals = 0;

    // to save output of get event function
    int err = -1;
    struct input_event currentEvent;

    // Entering interval-loop
    while (true)
    {
        intervalStart = millis();
        intervalStop = intervalStart + INTERVAL_LENGTH;
        // printf("Interval has started\n");

        while (true)
        {
            if (millis() > intervalStop)
            {
                break;
            }

            err = get_event(&currentEvent); // read all events during the interval

            if (err > -1 && currentEvent.type != EV_SYN && currentEvent.type != EV_MSC)
            {
                // Add current event to sum
                if (currentEvent.type == EV_REL)
                {
                    if (currentEvent.code == REL_X)
                    {
                        intervalX += currentEvent.value;
                        // printf("Recieved events: x->%d\n", currentEvent.value);
                    }
                    if (currentEvent.code == REL_Y)
                    {
                        intervalY += currentEvent.value;
                        // printf("Recieved events: y->%d\n", currentEvent.value);
                    }
                }
                else if (currentEvent.type == EV_KEY) // Register klicks right away
                {
                    emitKlick(currentEvent.value);
                }
            }
            else
            {
                continue;
            }
        }

        // TODO: do this before starting the first interval
        long long start = micros();

        // normalize the resampled event and add to buffer
        ResampledEvent resampledEvent;
        resampledEvent.x = ((float)intervalX + 31.0f) / 73.0f;
        resampledEvent.y = ((float)intervalY + 28.0f) / 59.0f;
        addEvent(&resampledEventsBuffer, resampledEvent);

        int ndims = 2;
        int64_t dims[] = {1, BUFFER_LENGTH};
        float dataX[BUFFER_LENGTH]; // these two arrays store the history of resampled events for x and y respectively
        float dataY[BUFFER_LENGTH];
        int ndata = BUFFER_LENGTH * sizeof(float); // This is tricky, it number of bytes not number of element
        /*
        printf("Circular Buffer: ");
        for (int i = 0; i < BUFFER_LENGTH; i++){
            printf("%f, ", resampledEventsBuffer.events[i].x);
        }
        printf("\nDataX: ");
        */
        // filling datax and y in the correct sequence
        for (int i = BUFFER_LENGTH - 1; i >= 0; i--)
        {
            int index = (resampledEventsBuffer.front - i + BUFFER_LENGTH - 1) % BUFFER_LENGTH;
            dataX[i] = resampledEventsBuffer.events[index].x;
            // printf("index: %d %f, ", index, dataX[i]);
            dataY[i] = resampledEventsBuffer.events[index].y;
        }

        // Create input tensors
        InputValues[0] = getTensor(ndims, dims, dataX, ndata);
        InputValues[1] = getTensor(ndims, dims, dataY, ndata);

        // //Run the Session
        TF_SessionRun(Session, NULL, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, NULL, 0, NULL, Status);
        if (TF_GetCode(Status) != TF_OK)
        {
            printf("%s", TF_Message(Status));
        }

        // get and de-normalize the output values
        float predX = getOutputValues(0, OutputValues) * 73.0f - 31.0f;
        float predY = getOutputValues(1, OutputValues) * 59.0f - 28.0f;

        if (dataX[BUFFER_LENGTH - 1] == 0)
        {
            // emit predicted events
            emit(EV_REL, REL_X, 0);
            emit(EV_REL, REL_Y, (int)roundf(predY));

            // TODO: Print for debugging purposes
            printf("Emitted events:     x->%d, y->%d\n", 0, predY);
        }
        if (dataY[BUFFER_LENGTH - 1] == 0)
        {
            // emit predicted events
            emit(EV_REL, REL_X, (int)roundf(predX));
            emit(EV_REL, REL_Y, 0);
            // TODO: Print for debugging purposes
            printf("Emitted events:     x->%d, y->%d\n", predX, 0);
        }
        emit(EV_SYN, SYN_REPORT, 0); // Syn-event

        // emit predicted events
        emitRel((int)roundf(predX), (int)roundf(predY));

        // TODO: for debugging
        // printf("Resampled event values: x %f, y %f\n", resampledEvent.x, resampledEvent.y);
        // printf("Buffer front: %d\n", resampledEventsBuffer.front);
        printf("Resampled events:   x->%d, y->%d\n", intervalX, intervalY);
        printf("Buffer front value: x %f, y %f\n", resampledEventsBuffer.events[resampledEventsBuffer.front].x, resampledEventsBuffer.events[resampledEventsBuffer.front].y);

        totalTime += micros() - start;
        intervals++;
        printf("Inference time %lld\n", totalTime / intervals);
        // reset for next interval
        intervalX = 0;
        intervalY = 0;
    }
    // onto the next interval...
}
