DESTDIR=/usr/lib/pixel_xcf

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

CFLAGS = $(DEBUG) $(OFLAGS) $(DEFS) `gtk-config --cflags`
INCLUDES = -I. -I.. \
		   -I/usr/include/tcl8.4 \
		   -I/usr/include/tcl8.4/tcl-private/generic 
LDFLAGS = -lm -ltcl8.4 -lxcf

all:    libpixel_xcf.so

libpixel_xcf.so:	main.o
	$(CC) --shared -o libpixel_xcf.so \
		main.o \
		$(LDFLAGS)

main.o:	main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)

install: all
	install -d $(DESTDIR)
	install libpixel_xcf.so $(DESTDIR)
	install pkgIndex.tcl $(DESTDIR)
	install pixel_xcf.tcl $(DESTDIR)

clean:
	-rm -f libpixel_xcf.so
	-rm -f *.o
	-rm -f core
        
