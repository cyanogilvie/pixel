DESTDIR=
TARGETDIR=$(DESTDIR)/usr/lib/pixel_sdl

CC=gcc

#OBJS = main.o timestuff.o widget.o
OBJS = main.o sdl_timestuff.o

OFLAGS0= -O0
OFLAGS1= -O9 -march=pentiumpro -mcpu=pentiumpro
OFLAGS2= -O9 -fforce-mem -fforce-addr -finline-functions \
-fkeep-inline-functions \
-ffast-math -fstrength-reduce -fthread-jumps -fcse-follow-jumps \
-fcse-skip-blocks -frerun-cse-after-loop -frerun-loop-opt \
-fexpensive-optimizations -fschedule-insns2 -fcaller-saves \
-funroll-loops -fmove-all-movables -fomit-frame-pointer \
-mcpu=i686 -march=i686 -malign-loops=4 -malign-jumps=4 \
-malign-functions=4
OFLAGS3= -O3

#OFLAGS=$(OFLAGS0)
OFLAGS=$(OFLAGS3)


DEFS =
DEBUG=-g
#DEBUG=-g -DDEBUG
INCLUDE = -I/usr/include/Pixel \
		   -I/usr/include/tcl8.4 \
		   -I/usr/include/tcl8.4/tcl-private/generic 
CFLAGS = -Wall $(DEBUG) $(OFLAGS) $(DEFS) $(INCLUDE) `sdl-config --cflags`


STATIC = 
LLIBS = 
LIBS =
LDFLAGS = `sdl-config --libs` -lGL -lGLU -lm

all:    libpixel_sdl.so

libpixel_sdl.so:	$(OBJS)
	$(CC) --shared -o libpixel_sdl.so $(OBJS) $(LDFLAGS)

main.o:	main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)

timestuff.o: timestuff.c
	$(CC) -c timestuff.c $(CFLAGS) $(INCLUDES)

widget.o: widget.c
	$(CC) -c widget.c $(CFLAGS) $(INCLUDES) $(TKFLAGS)

install: all
	install -d $(TARGETDIR)
	install libpixel_sdl.so $(TARGETDIR)
	install pkgIndex.tcl $(TARGETDIR)
	install pixel_sdl.tcl $(TARGETDIR)

clean:
	-rm -f libpixel_sdl.so
	-rm -f *.o
	-rm -f core
        
