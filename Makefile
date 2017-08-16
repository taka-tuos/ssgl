TARGET		= ssgl
OBJS_TARGET	= ssgl_c0.o use.o sdl.o mqoloader.o

CFLAGS += -O2 -g `sdl-config --cflags` -fpermissive
LIBS += `sdl-config --libs` -lm -lc -lstdc++

include Makefile.in
