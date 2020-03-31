OUTPUT = lavirint.out
CC      = gcc
CFLAGS  = -g -std=c99 -Wall -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib
LDLIBS  = -lglut -lGLU -lGL -lm
DEPS := $(wildcard src/*.h)
OBJ := $(patsubst %.c, %.o,$(wildcard src/*.c))

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OUTPUT): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

.PHONY: beauty clean dist

beauty:
	-indent -nut -kr main.c
	-rm *~ *BAK

clean:
	-rm src/*.o $(OUTPUT) 

dist: clean
	-tar -chvj -C .. -f ../$(OUTPUT).tar.bz2 $(OUTPUT)
