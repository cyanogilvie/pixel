DESTDIR=/usr/lib/pixel_jpeg

DEBUG=-g -DDEBUG
#DEBUG=
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
CC=gcc

OFLAGS=$(OFLAGS0)
#OFLAGS=$(OFLAGS2)

DEFS = \
	   -DHAVE_X11_DGA=1 \
	   -DPACKAGE=\"Xgfx2\" \
	   -DVERSION=\"1.0\"
DEFS := $(DEFS) -DASM_BUF_DRAW_BOX

CFLAGS = $(DEBUG) $(OFLAGS) $(DEFS)
INCLUDES = -I. -I.. \
		   -I/usr/include/tcl8.4 \
		   -I/usr/include/tcl8.4/tcl-private/generic 
LDFLAGS = -lm -ljpeg

all:    libpixel_jpeg.so

libpixel_jpeg.so:	main.o
	$(CC) --shared -o libpixel_jpeg.so \
		main.o \
		$(LDFLAGS)

main.o:	main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)

timestuff.o:	timestuff.c
	$(CC) -c timestuff.c $(CFLAGS) $(INCLUDES)

install: all
	install -d $(DESTDIR)
	install libpixel_jpeg.so $(DESTDIR)
	install pkgIndex.tcl $(DESTDIR)
	install pixel_jpeg.tcl $(DESTDIR)

clean:
	-rm -f libpixel_jpeg.so
	-rm -f out.jpg out2.jpg
	-rm -f *.o
	-rm -f core
        
