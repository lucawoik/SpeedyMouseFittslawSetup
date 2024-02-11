CC=gcc

main: main.c
	mkdir bin
	$(CC) -Wall -fcommon -I/usr/local/include/ -L/usr/local/lib main.c templates.c utils.c log.c predict.c -ltensorflow -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_ttf -lm -lpthread -lGL -o SpeedyMouseFittslaw

debug: main.c
	mkdir bin
	$(CC) -g -Wall -fcommon -I/usr/local/include/ -L/usr/local/lib main.c templates.c utils.c log.c predict.c -ltensorflow -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_ttf -lm -lpthread -lGL -o SpeedyMouseFittslawDebug

clean:
	rm -rf bin
