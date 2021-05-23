CFLAGS= -ggdb `sdl2-config --cflags`
CPPFLAGS= -ggdb `sdl2-config --cflags`
LDLIBS= `sdl2-config --libs` -lGL
CC=g++

hello:hello.o tbars.o esShader.o quad.o scene.o tridisplay.o projection.o

hello.o:hello.cpp

tbars.o:tbars.cpp

esShader.o:esShader.c

quad.o:quad.cpp

scene.o:scene.cpp

tridisplay.o:tridisplay.cpp

projection.o:projection.cpp
