# Structured makefile for camera placement
#
#

C++ = g++
LDLIBS =  -lGL -lGLEW -lglfw -lassimp
HEADERS = opengl.h structs.h shader.h mesh.h model.h
OBJS = model.o mesh.o shader.o

debug ?= n
ifeq ($(debug), y)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O2 -g
endif


all: pendulum

pendulum :	main.o $(OBJS) 
	$(C++) $(CFLAGS) main.o $(OBJS) -o pen $(LDLIBS)

main.o : main.cpp $(HEADERS)
	$(C++) $(CFLAGS) main.cpp -c

model.o : model.cpp structs.h shader.h mesh.h model.h
	$(C++) $(CFLAGS) model.cpp -c

mesh.o : mesh.cpp structs.h shader.h mesh.h
	$(C++) $(CFLAGS) mesh.cpp -c

shader.o : shader.cpp  structs.h shader.h
	$(C++) $(CFLAGS) shader.cpp -c

clean:
	rm *.o

pristine:
	rm *.o
	touch *
