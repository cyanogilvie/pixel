DESTDIR=
TARGETDIR=$(DESTDIR)/usr/lib/pixel_hermes

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

#OFLAGS=$(OFLAGS0)
OFLAGS=$(OFLAGS2)


DEFS =
DEBUG=-g
#DEBUG=-g -DDEBUG
INCLUDE = -I.. \
		   -I/usr/include/tcl8.3 \
		   -I/usr/include/tcl8.3/tcl-private/generic \
		   -I/usr/include/tcl8.3/tk-private/generic \
		   -I/usr/include/Hermes
CFLAGS = -Wall $(DEBUG) $(OFLAGS) $(DEFS) $(INCLUDE)


STATIC = 
LLIBS = 
LIBS =
LDFLAGS = -ltcl8.3 -ltk8.3 -lm -lHermes

all:    pixel_hermes.so

pixel_hermes.so:	$(OBJS)
	$(CC) --shared -o pixel_hermes.so $(OBJS) $(LDFLAGS)

main.o:	main.c
	$(CC) -c main.c $(CFLAGS) $(INCLUDES)

install: all
	install -d $(TARGETDIR)
	install pixel_hermes.so $(TARGETDIR)
	install pkgIndex.tcl $(TARGETDIR)
	install pixel_hermes.tcl $(TARGETDIR)

clean:
	-rm -f pixel_hermes.so
	-rm -f *.o
	-rm -f core
        
