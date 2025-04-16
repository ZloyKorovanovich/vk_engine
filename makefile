CC = GCC

INC_PATH = ext\inc
LIB_PATH = ext\lib

LIBS = -lvulkan-1 -lglfw3 -lgdi32 

CFLAGS = -O0 -g3 -Wall -I$(INC_PATH)
LDFLAGS = -L$(LIB_PATH)

all: vk_engine.exe

vk_engine.exe: main.o
	gcc $(LDFLAGS) -o out\bin\vk_engine.exe out\bin\main.o $(LIBS)

main.o:
	gcc $(CFLAGS) -c src\main.c -o out\bin\main.o

clean:
	del /q out\bin\main.o
	del /q out\bin\vk_engine.exe