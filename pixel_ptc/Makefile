DESTDIR=/usr/lib/pixel_ptc

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
CC=g++
#TKFLAGS=-I/src/cvs/tcl/tk/unix/../bitmaps -I/src/cvs/tcl/tk/generic -I/src/cvs/tcl/tcl/generic -I/usr/X11R6/include -DHAVE_UNISTD_H=1 -DHAVE_LIMITS_H=1 -D_LARGEFILE64_SOURCE=1 -DTCL_WIDE_INT_TYPE=long\ long -DHAVE_STRUCT_STAT64=1 -DHAVE_TYPE_OFF64_T=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TIME_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_PW_GECOS=1 -DTCL_NO_DEPRECATED -DUSE_TCL_STUBS
TKFLAGS=-I/usr/include/tcl8.3/tk-private/generic -I /usr/include/tcl8.3/tcl-private/generic -I/usr/X11R6/include -DHAVE_UNISTD_H=1 -DHAVE_LIMITS_H=1 -D_LARGEFILE64_SOURCE=1 -DTCL_WIDE_INT_TYPE=long\ long -DHAVE_STRUCT_STAT64=1 -DHAVE_TYPE_OFF64_T=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TIME_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_PW_GECOS=1 -DTCL_NO_DEPRECATED -DUSE_TCL_STUBS -ltk8.3

#OFLAGS=$(OFLAGS0)
OFLAGS=$(OFLAGS2)

DEFS = \
	   -DHAVE_X11_DGA=1 \
	   -DPACKAGE=\"Xgfx2\" \
	   -DVERSION=\"1.0\"
DEFS := $(DEFS) -DASM_BUF_DRAW_BOX

CFLAGS = -Wall $(DEBUG) $(OFLAGS) $(DEFS)
INCLUDES = -I. -I.. \
		   -I/usr/include/tcl8.3 \
		   -I/usr/include/tcl8.3/tcl-private/generic 
LDFLAGS = -L/usr/X11R6/lib -L. -lX11 -lXext -lXxf86dga -lXxf86vm -lm -ltcl8.3 -ltk8.3

all:    libpixel_ptc.so

libpixel_ptc.so:	main.o timestuff.o widget.o
	$(CC) --shared -o libpixel_ptc.so \
		main.o timestuff.o widget.o \
		/usr/lib/libptc.a \
		/usr/lib/libHermes.a \
		$(LDFLAGS)

main.o:	main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)

timestuff.o: timestuff.c
	$(CC) -c timestuff.c $(CFLAGS) $(INCLUDES)

widget.o: widget.c
	$(CC) -c widget.c $(CFLAGS) $(INCLUDES) $(TKFLAGS)

install: all
	install -d $(DESTDIR)
	install libpixel_ptc.so $(DESTDIR)
	install pkgIndex.tcl $(DESTDIR)
	install pixel_ptc.tcl $(DESTDIR)

clean:
	-rm -f libpixel_ptc.so
	-rm -f *.o
	-rm -f core
        
