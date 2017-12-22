CFLAGS += -g
LDFLAGS += -lm -lGL -lGLEW -lGLU -lglut
SOURCES += main.c geometry.c mesh.c utils.c
OBJECTS += $(patsubst %.c, %.o, $(SOURCES))

main: $(OBJECTS)

clean:
	$(RM) $(OBJECTS) main
