#include "main.h"

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
    eventZero.x = 0.430556f;
    eventZero.y = 0.474576f;

    // init buffer with all zeros to be able to call the ann right from the beginning
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        addEvent(buffer, eventZero);
        // printf("Filling buffer -> %d\n", i);
    }
}

// creates an input event for the specified device
// source: https://www.kernel.org/doc/html/v4.12/input/uinput.html
void emit(int fd, int type, int code, int val)
{
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;

    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}

void emitKlick(int fd, int value)
{
    emit(fd, EV_MSC, MSC_SCAN, 9001);
    emit(fd, EV_KEY, BTN_LEFT, value);
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

void emitRel(int fd, int x, int y)
{
    emit(fd, EV_REL, REL_X, x);
    emit(fd, EV_REL, REL_Y, y);
    emit(fd, EV_SYN, SYN_REPORT, 0); // Syn-event
    // TODO: Print for debugging purposes
    printf("Emitted events:     x->%d, y->%d\n", x, y);
}

// Input helpers
int initInput()
{
    // open the event handler
    int fd_event = open(EVENT_PATH, O_RDONLY | O_NONBLOCK); // | NONBLOCK in oder to make the read non-blocking
    if (fd_event == -1)
    {
        perror("Error opening evdev device");
        if (!IS_TEST_MODE)
            exit(0);
        return 0;
    }

    // this line reserves the device for this program so its events do not arrive at other applications
    ioctl(fd_event, EVIOCGRAB, 1);

    return fd_event;
}

int initUInput()
{
    // Create and open virtual uinput device
    struct uinput_user_dev uidev;

    // Open uinput device
    int fd_uinput = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd_uinput < 0)
    {
        perror("Error opening uinput device");
        if (!IS_TEST_MODE)
            exit(0);
        return 0;
    }

    // Set up the uinput device
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "virtual-mouse");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    ioctl(fd_uinput, UI_SET_EVBIT, EV_REL);
    ioctl(fd_uinput, UI_SET_RELBIT, REL_X);
    ioctl(fd_uinput, UI_SET_RELBIT, REL_Y);
    ioctl(fd_uinput, UI_SET_EVBIT, EV_KEY);
    ioctl(fd_uinput, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd_uinput, UI_SET_KEYBIT, BTN_RIGHT);

    write(fd_uinput, &uidev, sizeof(uidev));
    ioctl(fd_uinput, UI_DEV_CREATE);

    return fd_uinput;
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
    int fd_event = initInput();
    int fd_uinput = initUInput();
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

            struct input_event currentEvent;
            ssize_t bytesRead = read(fd_event, &currentEvent, sizeof(struct input_event)); // read all events during the interval

            if (bytesRead == sizeof(struct input_event))
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
                    emitKlick(fd_uinput, currentEvent.value);
                }
            }
            else
            {
                continue;
            }
        }

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
        for (int i = 0; i < BUFFER_LENGTH; i++)
        {
            int index = (resampledEventsBuffer.front - i + BUFFER_LENGTH) % BUFFER_LENGTH;
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

        // emit predicted events
        emitRel(fd_uinput, (int)roundf(predX), (int)roundf(predY));

        // normalize the resampled event and add to buffer
        ResampledEvent resampledEvent;
        resampledEvent.x = ((float)intervalX + 31.0f) / 72.0f;
        resampledEvent.y = ((float)intervalY + 28.0f) / 59.0f;
        addEvent(&resampledEventsBuffer, resampledEvent);

        // TODO: for debugging
        // printf("Resampled event values: x %f, y %f\n", resampledEvent.x, resampledEvent.y);
        // printf("Buffer front: %d\n", resampledEventsBuffer.front);
        printf("Resampled events:   x->%d, y->%d\n", intervalX, intervalY);
        printf("Buffer front value: x %f, y %f\n", resampledEventsBuffer.events[resampledEventsBuffer.front].x, resampledEventsBuffer.events[resampledEventsBuffer.front].y);

        // reset for next interval
        intervalX = 0;
        intervalY = 0;
    }
    // onto the next interval...
}
