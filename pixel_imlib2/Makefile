DESTDIR=
TARGETDIR=/usr/lib/pixel_imlib2

TARGET = pixel_imlib2.so

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
OFLAGS= -O3

#OFLAGS=$(OFLAGS0)
OFLAGS=$(OFLAGS3)


DEFS =
DEBUG=-g
#DEBUG=-g -DDEBUG
INCLUDE = -I.. \
		   -I/usr/include/tcl8.4 \
		   -I/usr/include/tcl8.4/tcl-private/generic 
CFLAGS = -Wall $(DEBUG) $(OFLAGS) $(DEFS) $(INCLUDE) `imlib2-config --cflags`


STATIC = 
LLIBS = 
LIBS =
LDFLAGS = `imlib2-config --libs` -lm

all:    $(TARGET)

$(TARGET): $(OBJS)
	$(CC) --shared -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

install: all
	install -d $(TARGETDIR)
	install $(TARGET) $(TARGETDIR)
	install pkgIndex.tcl $(TARGETDIR)
	install pixel_imlib2.tcl $(TARGETDIR)

clean:
	-rm -f $(TARGET) core *.o test_save.*

