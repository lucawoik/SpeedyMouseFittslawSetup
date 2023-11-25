CC=gcc

main: main.c
	mkdir bin
# $(CC) -Wall -c main.c -lSDL2main -lSDL2 -lSDL2_gfx -lm -lpthread -o bin/main.o
# $(CC) -Wall -c templates.c -lSDL2main -lSDL2 -lSDL2_gfx -lm -lpthread -o bin/templates.o
# $(CC) -Wall -c utils.c -lSDL2main -lSDL2 -lSDL2_gfx -lm -lpthread -o bin/utils.o
# $(CC) -Wall -c log.c -lSDL2main -lSDL2 -lSDL2_gfx -lm -lpthread -o bin/log.o
# $(CC) -Wall bin/main.o bin/templates.o bin/utils.o bin/log.o -lSDL2main -lSDL2 -lSDL2_gfx -lm -lpthread -o Fittsi
	$(CC) -Wall -fcommon main.c templates.c utils.c log.c -lSDL2main -lSDL2 -lSDL2_gfx -lm -lpthread -o FittsiM

clean:
	rm -rf bin
	rm -rf log


#	$(CC) -Wall main.c -I/usr/include/SDL2 -D_REENTRANT -lSDL2main -lSDL2 -lSDL2_gfx -lm -o bin/main.o
#	$(CC) -Wall bin/main.o -I/usr/include/SDL2 -D_REENTRANT -lSDL2main -lSDL2 -lSDL2_gfx -lm -o Fittsi

