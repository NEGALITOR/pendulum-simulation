C++ = g++
CFLAGS = -I/usr/include/freetype2/
LDLIBS = -lGL -lGLEW -lglfw -lassimp -lfreetype
HEADERS = opengl.h globals.h structs.h shader.h mesh.h model.h phaseSpacePlot.h rungeKutta.h textRender.h light.h
OBJS = model.o mesh.o shader.o rungeKutta.o phaseSpacePlot.o textRender.o light.o
soil = ./includes/SOIL2/libsoil2.a

debug ?= n
ifeq ($(debug), y)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O2 -g
endif

all: pendulum

pendulum :	main.o $(OBJS) 
	$(C++) $(CFLAGS) main.o $(OBJS) -o pen $(LDLIBS) $(soil)

main.o : main.cpp $(HEADERS)
	$(C++) $(CFLAGS) main.cpp -c $(soil)

shader.o : shader.cpp structs.h shader.h
	$(C++) $(CFLAGS) shader.cpp -c

model.o : model.cpp structs.h shader.h mesh.h model.h
	$(C++) $(CFLAGS) model.cpp -c

mesh.o : mesh.cpp structs.h shader.h mesh.h
	$(C++) $(CFLAGS) mesh.cpp -c

light.o : light.cpp light.h shader.h
	$(C++) $(CFLAGS) light.cpp -c

rungeKutta.o : rungeKutta.cpp globals.h structs.h rungeKutta.h
	$(C++) $(CFLAGS) rungeKutta.cpp -c

phaseSpacePlot.o : phaseSpacePlot.cpp globals.h structs.h phaseSpacePlot.h
	$(C++) $(CFLAGS) phaseSpacePlot.cpp -c

textRender.o : textRender.cpp textRender.h
	$(C++) $(CFLAGS) textRender.cpp -c

clean:
	rm -f *.o pen

pristine:
	rm -f *.o pen
	touch *
