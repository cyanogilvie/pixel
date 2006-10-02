DESTDIR =
BASE = usr
TARGETDIR = $(DESTDIR)/$(BASE)/lib/pixel_tkimage
TCLVER = 8.4

CC=gcc

OBJS = main.o

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
OFLAGS3= -O6 -march=pentium3 -mtune=pentium3 -fexceptions -funit-at-a-time

#OFLAGS=$(OFLAGS0)
OFLAGS=$(OFLAGS3)


DEFS =
DEBUG=-g
#DEBUG=-g -DDEBUG
#INCLUDE = -I.. \
		   -I/usr/include/tcl8.3 \
		   -I/usr/include/tcl8.3/tcl-private/generic \
		   -I/usr/include/tcl8.3/tk-private/generic \
		   -I/usr/include/Hermes
#INCLUDE = -I.. \
		   -I/$(BASE)/include \
		   -I/src/cvs/tcl/tk/generic \
		   -I/src/cvs/tcl/tk/unix \
		   -I/src/cvs/tcl/tcl/generic \
		   -I/src/cvs/tcl/tcl/unix \
		   -I/usr/include/Hermes
INCLUDE = -I.. \
		   -I/$(BASE)/include \
		   -I/usr/include/tcl8.4 \
		   -I/usr/include/tcl8.4/tcl-private/generic \
		   -I/usr/include/tcl8.4/tk-private/generic \
		   -I/usr/include/Hermes
CFLAGS = -Wall $(DEBUG) $(OFLAGS) $(DEFS) $(INCLUDE)


STATIC = 
LLIBS = -L/$(BASE)/lib
LIBS = -ltcl$(TCLVER) -ltk$(TCLVER) -lm -lHermes
LDFLAGS = -Wall $(STATIC) $(LLIBS) $(LIBS)

all:    pixel_tkimage.so

pixel_tkimage.so:	$(OBJS)
	$(CC) --shared -o pixel_tkimage.so $(OBJS) $(LDFLAGS)

main.o:	main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)

install: all
	install -d $(TARGETDIR)
	install pixel_tkimage.so $(TARGETDIR)
	install pkgIndex.tcl $(TARGETDIR)
	install pixel_tkimage.tcl $(TARGETDIR)

clean:
	-rm -f pixel_tkimage.so
	-rm -f *.o
	-rm -f core

