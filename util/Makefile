CC=gcc
PKG=allegro-5.0 \
    allegro_image-5.0 \
    allegro_font-5.0 \
    allegro_ttf-5.0

CFLAGS=-O2 -Wall -ggdb `pkg-config --cflags ${PKG}`
LDLIBS=-lm `pkg-config --libs ${PKG}`
LDFLAGS=-O2

# To add more programs, put the name here and make an entry at the bottom.
EXECUTABLES=ttf2png


OBJECTS=$(addsuffix .o, $(EXECUTABLES))

all: $(EXECUTABLES)

clean:
	rm -f $(EXECUTABLES) $(OBJECTS)

ttf2png: ttf2png.o

.PHONY: all clean
