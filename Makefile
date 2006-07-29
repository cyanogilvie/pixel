DESTDIR =
#BASE = tcl8.4
BASE = usr
TCLVER = 8.4

MAJ = 1
MIN = 0

LIBNAME = libpixel
#SONAME = libpixel.so.$(MAJ)
#TARGET = $(SONAME).$(MIN)
TARGET = $(LIBNAME).so

TCL_OBJS = \
		   main.o \
		   tcl_pmap.o \
		   tcl_ttf_face.o

OBJS = \
	   2d.o \
	   misc.o \
	   primitives.o \
	   ttf.o \
	   shaders.o \
	   bezierstuff.o \
	   rle.o

HDRS = \
	   2d.h \
	   tcl_pmap.h \
	   primitives.h \
	   misc.h

MODULES = \
	pixel_jpeg \
	pixel_fonts \
	pixel_png \
	pixel_tkutils \
	pixel_sdl \
	pixel_tiff \
	pixel_imlib2 \
	pixel_svg_cairo

DISABLED_MODULES = \
	pixel_hermes \
	pixel_xcf \
	pixel_rsvg \
	pixel_ptc

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
OFLAGS3= -march=pentium3 -mtune=pentium3 -O9 -fexceptions -funit-at-a-time
OFLAGS4= -O9 -fexceptions

OFLAGS=$(OFLAGS0)
#OFLAGS=$(OFLAGS3)

DEFS = \
	   -DVERSION=\"1.0\"
DEFS := $(DEFS) -DASM_BUF_DRAW_BOX

#INCLUDES = `freetype-config --cflags` \
		   -I/$(BASE)/include/tcl$(TCLVER) \
		   -I/$(BASE)/include/tcl$(TCLVER)/tcl-private/generic 
INCLUDES = `freetype-config --cflags` \
		   -I/$(BASE)/include \
		   -I/usr/include/tcl8.4
CFLAGS = -Wall $(DEBUG) $(OFLAGS) $(DEFS) $(INCLUDES)


STATIC =
LLIBS = -L/usr/X11R6/lib -L/$(BASE)/lib
LIBS = $(LLIBS) `freetype-config --libs` -lm
LDFLAGS = $(STATIC) $(LIBS)


GCC = gcc
GXX = g++
AR = ar -rs


all:    $(TARGET) libpixel_base.so scripts
	@for i in $(MODULES); do \
		echo "====> make $@ in $$i"; \
		make -C $$i $@ || exit 1; \
	done

$(TARGET): libpixel_base.a $(TCL_OBJS)
	$(GCC) --shared -o $(TARGET) $(TCL_OBJS) $(LDFLAGS) libpixel_base.a

gch: *.h
	$(CC) -c all.h $(INCLUDES)

ctags:
	ctags -R * /usr/include/tcl8.3 /usr/include/ptc /usr/include/SDL /usr/include/Imlib2.h

%.o: %.cc
	$(GCC) -c $< $(CFLAGS)

misc.o:	misc.asm
	nasm -f elf -g misc.asm

scripts: scripts-stamp

libpixel_base.so: $(OBJS)
	$(CC) --shared -o libpixel_base.so $(OBJS) `freetype-config --libs`

libpixel_base.a: $(OBJS)
	$(AR) libpixel_base.a $(OBJS)

scripts-stamp: scripts/*.itcl
	./make_tclIndex.tcl
	touch scripts-stamp

install: all
	install -d $(DESTDIR)/$(BASE)/lib/pixel
	install -d $(DESTDIR)/$(BASE)/include/pixel
	install -d $(DESTDIR)/$(BASE)/lib/pixel/scripts
	install $(TARGET) $(DESTDIR)/$(BASE)/lib/pixel
	install libpixel_base.a $(DESTDIR)/$(BASE)/lib
	install libpixel_base.so $(DESTDIR)/$(BASE)/lib
	install pkgIndex.tcl $(DESTDIR)/$(BASE)/lib/pixel
	install pixel.tcl $(DESTDIR)/$(BASE)/lib/pixel
#	install bg.tiff $(DESTDIR)/$(BASE)/lib/pixel
#	install block.tiff $(DESTDIR)/$(BASE)/lib/pixel
	install test.tcl $(DESTDIR)/$(BASE)/lib/pixel
	install scripts/*.itcl $(DESTDIR)/$(BASE)/lib/pixel/scripts
	install scripts/tclIndex $(DESTDIR)/$(BASE)/lib/pixel/scripts
	cp $(HDRS) $(DESTDIR)/$(BASE)/include/pixel
	@for i in $(MODULES); do \
		echo "====> make $@ in $$i"; \
		make -C $$i $@ || exit 1; \
	done

clean:
	-rm -f libpixel.so*
	-rm -f libpixel_base.*
	-rm -f *.gch
	-rm -f *.o
	-rm -f core
#	-rm -f scripts/tclIndex scripts-stamp
	@for i in $(MODULES); do \
		echo "====> make $@ in $$i"; \
		make -C $$i $@ || exit 1; \
	done

