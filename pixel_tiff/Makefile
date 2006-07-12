DESTDIR=/usr/lib/pixel_tiff

DEBUG=-g -DDEBUG
#DEBUG=
OFLAGS0=
OFLAGS1= -O9 -march=pentiumpro -mcpu=pentiumpro
OFLAGS2= -O9 -fforce-mem -fforce-addr -finline-functions \
-fkeep-inline-functions \
-ffast-math -fstrength-reduce -fthread-jumps -fcse-follow-jumps \
-fcse-skip-blocks -frerun-cse-after-loop -frerun-loop-opt \
-fexpensive-optimizations -fschedule-insns2 -fcaller-saves \
-funroll-loops -fmove-all-movables -fomit-frame-pointer \
-mcpu=i686 -march=i686 -malign-loops=4 -malign-jumps=4 \
-malign-functions=4
OFLAGS= -O3

CC=gcc

#OFLAGS=$(OFLAGS0)
OFLAGS=$(OFLAGS3)

DEFS = \
	   -DHAVE_X11_DGA=1 \
	   -DPACKAGE=\"Xgfx2\" \
	   -DVERSION=\"1.0\"
DEFS := $(DEFS) -DASM_BUF_DRAW_BOX

CFLAGS = $(DEBUG) $(OFLAGS) $(DEFS)
INCLUDES = -I. -I.. \
		   -I/usr/include/tcl8.4 \
		   -I/usr/include/tcl8.4/tcl-private/generic 
LDFLAGS = -lm -ltiff

all:    libpixel_tiff.so

libpixel_tiff.so:	main.o
	$(CC) --shared -o libpixel_tiff.so \
		main.o \
		$(LDFLAGS)

main.o:	main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)

timestuff.o:	timestuff.c
	$(CC) -c timestuff.c $(CFLAGS) $(INCLUDES)

install: all
	install -d $(DESTDIR)
	install libpixel_tiff.so $(DESTDIR)
	install pkgIndex.tcl $(DESTDIR)
	install pixel_tiff.tcl $(DESTDIR)

clean:
	-rm -f libpixel_tiff.so
	-rm -f *.o
	-rm -f core
	-rm -f out.tiff
        
