DESTDIR =

MAJ = 1
MIN = 0

LIBNAME = libpixel
#SONAME = libpixel.so.$(MAJ)
#TARGET = $(SONAME).$(MIN)
TARGET = $(LIBNAME).so

OBJS = \
	   main.o \
	   2d.o \
	   misc.o \
	   primitives.o \
	   ttf.o \
	   tcl_ttf_face.o \
	   shaders.o \
	   bezierstuff.o \
	   tcl_pmap.o

MODULES = \
	pixel_jpeg \
	pixel_sdl \
	pixel_tiff \
	pixel_xcf \
	pixel_ptc \
	pixel_imlib2 \
	pixel_tkimage \
	pixel_hermes

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

#OFLAGS=$(OFLAGS0)
OFLAGS=$(OFLAGS2)

DEFS = \
	   -DVERSION=\"1.0\"
DEFS := $(DEFS) -DASM_BUF_DRAW_BOX

INCLUDES = `freetype-config --cflags` \
		   -I. -Ilib -I/test/include \
		   -I/usr/include/tcl8.3 \
		   -I/usr/include/tcl8.3/tcl-private/generic 
CFLAGS = -Wall $(DEBUG) $(OFLAGS) $(DEFS) $(INCLUDES)


STATIC =
LLIBS = -L/usr/X11R6/lib
LIBS = $(LLIBS) `freetype-config --libs` -lm -ltcl8.3
LDFLAGS = $(STATIC) $(LIBS)


GCC = gcc
GXX = g++


all:    $(TARGET)
	@for i in $(MODULES); do \
		echo "====> make $@ in $$i"; \
		make -C $$i $@ || exit 1; \
	done

$(TARGET): $(OBJS)
	$(GCC) --shared -o $(TARGET) $(OBJS) $(LDFLAGS)


ctags:
	ctags -R * /usr/include/tcl8.3 /usr/include/ptc /usr/include/SDL /usr/include/Imlib2.h

%.o: %.cc
	$(GCC) -c $< $(CFLAGS)

bezierstuff.o: bezierstuff.c
	$(GXX) -c bezierstuff.c $(CFLAGS)

misc.o:	misc.asm
	nasm -f elf misc.asm

scripts: scripts-stamp

scripts-stamp: scripts/*.tcl
	./make_tclIndex.tcl
	touch scripts-stamp

install: all
	install -d $(DESTDIR)/usr/lib/pixel
	install -d $(DESTDIR)/usr/lib/pixel/scripts
	install $(TARGET) $(DESTDIR)/usr/lib/pixel
	install pkgIndex.tcl $(DESTDIR)/usr/lib/pixel
	install pixel.tcl $(DESTDIR)/usr/lib/pixel
#	install bg.tiff $(DESTDIR)/usr/lib/pixel
#	install block.tiff $(DESTDIR)/usr/lib/pixel
	install test.tcl $(DESTDIR)/usr/lib/pixel
#	install scripts/*.tcl $(DESTDIR)/usr/lib/pixel/scripts
#	install scripts/tclIndex $(DESTDIR)/usr/lib/pixel/scripts
	@for i in $(MODULES); do \
		echo "====> make $@ in $$i"; \
		make -C $$i $@ || exit 1; \
	done

clean:
	-rm -f libpixel.so*
	-rm -f *.o
	-rm -f core
#	-rm -f scripts/tclIndex scripts-stamp
	@for i in $(MODULES); do \
		echo "====> make $@ in $$i"; \
		make -C $$i $@ || exit 1; \
	done

