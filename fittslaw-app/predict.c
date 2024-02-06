#include "main.h"

void *manipulateMouseEvents(void *arg){
    // open the event handler
    fd = open(EVENT_PATH, O_RDONLY); // | NONBLOCK in oder to make the read non-blocking
    if (fd == -1)
    {
        perror("Error opening evdev device");
        if (!IS_TEST_MODE)
            exit(0);
        return NULL;
    }

    while(true)
    {
        long startTime2 = millis();
        long intervalEnd = startTime2 + 1000;
        printf("started at %lu\n", startTime2);
        printf("ends at %lu\n", intervalEnd);
        while(millis() < intervalEnd){
            printf("waiting...\n");
            usleep(100*1000);
        }
        printf("One Second has passed...\n");
    }

    // Schleife über n ms
        // Events lesen, resampeln (durschnitt aus allen Events) und speichern
            // Wenn keine events reinkommen, 0-event speichern
        // eigentliche Events auf 0 setzten oder "schlucken"
    
    // Array/Liste der letzen "250" Intervalle um so viele Events wie reingekommen sind nach rechts shifen und neue Events vorne anhängen 

    // ML-Modell aufrufen mit Intervall-Array/liste

    // Event mit Ergebnis schreiben und schicken (ggf. loggen)

}