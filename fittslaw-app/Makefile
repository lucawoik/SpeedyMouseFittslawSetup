CC=gcc

LINKER = -I/usr/include/libevdev-1.0/ -I/usr/local/include/ -L/usr/local/lib
FLAGS = -ltensorflow -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_ttf -lm -lpthread -lGL -levdev

main: main.c
	mkdir bin
	$(CC) -Wall -fcommon $(LINKER) main.c templates.c utils.c log.c predict.c $(FLAGS) -o SpeedyMouseFittslaw

debug: main.c
	mkdir bin
	$(CC) -Wall -fcommon $(LINKER) main.c templates.c utils.c log.c predict.c $(FLAGS) -o SpeedyMouseFittslawDebug

clean:
	rm -rf bin
