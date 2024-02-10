#include "main.h"

int intervalDurationMs = 250;

int eventsInInterval = 0;

long intervalStart = 0;
long intervalStop = 0;

int intervalX = 0;
int intervalY = 0;

typedef struct
{
    float x;
    float y;
} ResampledEvent;

typedef struct
{
    ResampledEvent events[200];
    int front;
} CircularBuffer;

CircularBuffer resampledEventsBuffer;

// Helper function for accessing Tensorflow C API
void NoOpDeallocator(void *data, size_t a, void *b) {}

// Functions for circular buffer
void addEvent(CircularBuffer *buffer, ResampledEvent event)
{
    buffer->front = (buffer->front + 1) % 200;

    buffer->events[buffer->front] = event;
}

void initCircularBuffer(CircularBuffer *buffer)
{
    buffer->front = -1;

    ResampledEvent eventZero;
    eventZero.x = 0.5f;
    eventZero.y = 0.5f;

    for (int i = 0; i < 200; i++)
    {
        addEvent(buffer, eventZero);
        printf("Filling buffer -> %d\n", i);
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

void *manipulateMouseEvents(void *arg)
{
    //********* Read model
    TF_Graph *Graph = TF_NewGraph();
    TF_Status *Status = TF_NewStatus();

    TF_SessionOptions *SessionOpts = TF_NewSessionOptions();
    TF_Buffer *RunOpts = NULL;

    const char *saved_model_dir = "ANN_10ms_200_zeros/"; // Path of the model
    const char *tags = "serve";                        // default model serving tag; can change in future
    int ntags = 1;

    TF_Session *Session = TF_LoadSessionFromSavedModel(SessionOpts, RunOpts, saved_model_dir, &tags, ntags, Graph, NULL, Status);
    if (TF_GetCode(Status) == TF_OK)
    {
        printf("TF_LoadSessionFromSavedModel OK\n");
    }
    else
    {
        printf("%s", TF_Message(Status));
    }

    //****** Get input tensor
    int NumInputs = 2;
    TF_Output *Input = malloc(sizeof(TF_Output) * NumInputs);

    TF_Output t0 = {TF_GraphOperationByName(Graph, "serving_default_last_x_values"), 0};
    if (t0.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName serving_default_batch_normalization_input\n");
    else
        printf("TF_GraphOperationByName serving_default_batch_normalization_input is OK\n");

    Input[0] = t0;

    TF_Output t1 = {TF_GraphOperationByName(Graph, "serving_default_last_y_values"), 0};
    if (t1.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName serving_default_batch_normalization_input\n");
    else
        printf("TF_GraphOperationByName serving_default_batch_normalization_input is OK\n");

    Input[1] = t1;

    //********* Get Output tensor
    int NumOutputs = 2;
    TF_Output *Output = malloc(sizeof(TF_Output) * NumOutputs);

    TF_Output t2 = {TF_GraphOperationByName(Graph, "StatefulPartitionedCall"), 0};
    if (t2.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName StatefulPartitionedCall\n");
    else
        printf("TF_GraphOperationByName StatefulPartitionedCall is OK\n");

    Output[0] = t2;

    TF_Output t3 = {TF_GraphOperationByName(Graph, "StatefulPartitionedCall"), 1};
    if (t3.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName StatefulPartitionedCall\n");
    else
        printf("TF_GraphOperationByName StatefulPartitionedCall is OK\n");

    Output[1] = t3;
    //********* Allocate data for inputs & outputs
    TF_Tensor **InputValues = (TF_Tensor **)malloc(sizeof(TF_Tensor *) * NumInputs);
    TF_Tensor **OutputValues = malloc(sizeof(TF_Tensor *) * NumOutputs);

    // #######

    int fd_event = initInput();
    int fd_uinput = initUInput();
    initCircularBuffer(&resampledEventsBuffer);

    printf("Circular Buffer contents:\n");
    for (int i = 0; i < 200; i++)
    {
        int index = (resampledEventsBuffer.front - i + 200) % 200;
        printf("%d  x: %f, y: %f\n", i, resampledEventsBuffer.events[index].x, resampledEventsBuffer.events[index].y);
    }

    while (true)
    {
        intervalStart = millis();
        intervalStop = intervalStart + intervalDurationMs;
        // printf("Interval has started\n");

        while (true)
        {
            if (millis() > intervalStop)
            {
                break;
            }

            struct input_event currentEvent;

            // read all events during the interval
            ssize_t bytesRead = read(fd_event, &currentEvent, sizeof(struct input_event));

            // Counting events to keep track of the number of events already saved
            if (bytesRead == sizeof(struct input_event))
            {
                eventsInInterval++;
                // Add current event to sum
                if (currentEvent.type == EV_REL)
                {
                    if (currentEvent.code == REL_X)
                    {
                        intervalX += currentEvent.value;
                        printf("Recieved events: x->%d\n", currentEvent.value);
                    }
                    if (currentEvent.code == REL_Y)
                    {
                        intervalY += currentEvent.value;
                        printf("Recieved events: y->%d\n", currentEvent.value);
                    }
                }
            }
            else
            {
                continue;
            }
        }

        int ndims = 2;
        int64_t dims[] = {1, 200};
        float dataX[200];
        float dataY[200];
        printf("Circular Buffer: ");
        for (int i = 0; i < 200; i++){
            printf("%f, ", resampledEventsBuffer.events[i].x);
        }
        printf("\n");
        printf("\n");
        printf("DataX: ");
        for (int i = 0; i < 200; i++)
        {
            int index = (resampledEventsBuffer.front - i + 200) % 200;
            dataX[i] = resampledEventsBuffer.events[index].x;
            printf("index: %d %f, ", index, dataX[i]);
            dataY[i] = resampledEventsBuffer.events[index].y;
        }
        printf("\n");
        int ndata = 200 * sizeof(float); // This is tricky, it number of bytes not number of element

        TF_Tensor *int_tensor = TF_NewTensor(TF_FLOAT, dims, ndims, dataX, ndata, &NoOpDeallocator, 0);
        if (int_tensor != NULL)
        {
            printf("TF_NewTensor is OK\n");
        }
        else
            printf("ERROR: Failed TF_NewTensor\n");

        InputValues[0] = int_tensor;

        TF_Tensor *int_tensor1 = TF_NewTensor(TF_FLOAT, dims, ndims, dataY, ndata, &NoOpDeallocator, 0);
        if (int_tensor1 != NULL)
        {
            printf("TF_NewTensor is OK\n");
        }
        else
            printf("ERROR: Failed TF_NewTensor\n");

        InputValues[1] = int_tensor1;
        // //Run the Session
        TF_SessionRun(Session, NULL, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, NULL, 0, NULL, Status);

        if (TF_GetCode(Status) == TF_OK)
        {
            printf("Session is OK\n");
        }
        else
        {
            printf("%s", TF_Message(Status));
        }

        void *buff = TF_TensorData(OutputValues[0]);
        float *offsets = buff;

        void *buff1 = TF_TensorData(OutputValues[1]);
        float *offsets1 = buff1;

        float predX = offsets[0]*73.0f-31.0f;
        float predY = offsets1[0]*59.0f-28.0f;

        emit(fd_uinput, EV_REL, REL_X, (int)predX);
        emit(fd_uinput, EV_REL, REL_Y, (int)predY);
        emit(fd_uinput, EV_SYN, SYN_REPORT, 0);

        printf("Emitted events: x->%f, y->%f\n", predX, predY);

        ResampledEvent resampledEvent;
        resampledEvent.x = (intervalX+31.0f)/72.0f;
        resampledEvent.y = (intervalY+28.0f)/59.0f;
        printf("Resampled event values: x %f, y %f\n", resampledEvent.x, resampledEvent.y);


        addEvent(&resampledEventsBuffer, resampledEvent);

        printf("Buffer front: %d\n", resampledEventsBuffer.front);
        printf("Buffer front value: x %f, y %f\n", resampledEventsBuffer.events[resampledEventsBuffer.front].x, resampledEventsBuffer.events[resampledEventsBuffer.front].y);


        // reset for next interval
        eventsInInterval = 0;
        intervalX = 0;
        intervalY = 0;
    }

    // Schleife über n ms
    // Events lesen, resampeln (durschnitt aus allen Events) und speichern
    // Wenn keine events reinkommen, 0-event speichern
    // eigentliche Events auf 0 setzten oder "schlucken"

    // Array/Liste der letzen "250" Intervalle um so viele Events wie reingekommen sind nach rechts shifen und neue Events vorne anhängen

    // ML-Modell aufrufen mit Intervall-Array/liste

    // Event mit Ergebnis schreiben und schicken (ggf. loggen)
}
