DESTDIR = 
TARGETDIR = $(DESTDIR)/usr/lib/pixel_tkutils

SOEXT = so
CC = gcc

LIBNAME = pixel_tkutils
TARGET = $(LIBNAME).$(SOEXT)

OBJS = \
	   main.o

STATIC = 
LLIBS = 
LIBS = 
LDFLAGS = -Wall $(STATIC) $(LLIBS) $(LIBS)

DEFINES =
OFLAGS = -O3
DEBUG = -g
INCLUDES = -I/usr/include/tcl8.3
CFLAGS = $(DEFINES) $(OFLAGS) $(DEBUG) $(INCLUDES)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) --shared -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	-rm -f *.o core $(TARGET)

install:
	install -d $(TARGETDIR)
	install $(TARGET) $(TARGETDIR)
	install pkgIndex.tcl $(TARGETDIR)
	install pixel_tkutils.tcl $(TARGETDIR)
