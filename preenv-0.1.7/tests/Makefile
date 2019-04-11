APPS:=basic location paths

SDL_CFLAGS:=$(shell sdl-config --cflags)
SDL_LIBS:=$(shell sdl-config --libs)
PDK_CFLAGS:=-I../libsdlpre
PDK_LIBS:=-L../libsdlpre -lsdlpre -lGLES_CM

all: $(APPS)

%: %.c
	$(CC) $(SDL_CFLAGS) $(PDK_CFLAGS) $(CFLAGS) -o $@ $< $(SDL_LIBS) $(PDK_LIBS)

clean:
	rm -f $(APPS)
	rm -f *.o

.PHONY: all clean

