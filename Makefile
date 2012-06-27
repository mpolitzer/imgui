CC=gcc
PKG=gl glu allegro-5.0 allegro_image-5.0

CFLAGS=-O2 -Wall -ggdb -funroll-loops -c `pkg-config --cflags ${PKG}`
LDLIBS=-lm `pkg-config --libs ${PKG}`
LDFLAGS=-O2

SOURCES=main.c ui.c
HEADERS=
EXECUTABLE=main

OBJECTS=$(addsuffix .o, $(basename ${SOURCES}))

all: $(EXECUTABLE)

zip: Makefile $(SOURCES) $(HEADERS)
	zip -r mypackage.zip $^

$(EXECUTABLE): $(OBJECTS)

$(OBJECTS): %.o: %.c $(HEADERS)

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

.PHONY: all clean
