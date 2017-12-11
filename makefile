LDFLAGS += -lm -lGL -lGLEW -lGLU -lglut -g

main: main.o geometry.o mesh.o

clean:
	$(RM) main.o geometry.o mesh.o main
