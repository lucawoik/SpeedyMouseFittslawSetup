#include "main.h"

int intervalDurationMs = 5;

int eventsInInterval = 0;

long intervalStart = 0;
long intervalStop = 0;

int intervalX = 0;
int intervalY = 0;

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

int initInput(){
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

int initUInput(){
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
    int fd_event = initInput();
    int fd_uinput = initUInput();

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

        emit(fd_uinput, EV_REL, REL_X, intervalX);
        emit(fd_uinput, EV_REL, REL_Y, intervalY);
        emit(fd_uinput, EV_SYN, SYN_REPORT, 0);

        printf("Emitted events: x->%d, y->%d\n", intervalX, intervalY);

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