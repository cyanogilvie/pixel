DESTDIR =
TARGETDIR = $(DESTDIR)/usr/lib/pixel_png

CC = gcc
SOEXT = so
TARGET = pixel_png.$(SOEXT)

OBJS = \
	   main.o \
	   pngstuff.o

DEBUG=-g -DDEBUG
#DEBUG=
OFLAGS0= -O0
OFLAGS1= -O3 -march=pentium3 -mtune=pentium3

OFLAGS=$(OFLAGS0)
DEFS = 
INCLUDES = -I. -I.. \
		   -I/usr/include/tcl8.3 \
		   -I/usr/include/tcl8.3/tcl-private/generic 
CFLAGS = $(DEBUG) $(OFLAGS) $(DEFS) $(INCLUDES)


STATIC =
LLIBS = 
LIBS = -ltcl8.3 -lpng
LDFLAGS = -Wall $(STATIC) $(LLIBS) $(LIBS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) --shared -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

install: all
	install -d $(TARGETDIR)
	install $(TARGET) $(TARGETDIR)
	install pkgIndex.tcl $(TARGETDIR)
	install pixel_png.tcl $(TARGETDIR)

clean:
	-rm -f $(TARGET)
	-rm -f out.png
	-rm -f *.o
	-rm -f core
        
