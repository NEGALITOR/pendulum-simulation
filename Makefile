# Structured makefile for camera placement
#
#

C++ = g++
LDLIBS =  -lGL -lGLEW -lglfw
HEADERS = opengl.h structs.h
OBJS = loadmodel.o model.o

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

model.o : model.cpp opengl.h structs.h
	$(C++) $(CFLAGS) model.cpp -c

clean:
	rm *.o

pristine:
	rm *.o
	touch *
