DESTDIR=
TARGETDIR=$(DESTDIR)/usr/lib/pixel_fonts

all: scripts

scripts: scripts-stamp

scripts-stamp: scripts/*.itcl
	./make_tclIndex.tcl
	touch scripts-stamp

install: all
	install -d $(TARGETDIR)
	install -d $(TARGETDIR)/scripts
	install -d $(TARGETDIR)/fonts
	install pkgIndex.tcl $(TARGETDIR)
	install fonts.tcl $(TARGETDIR)
	install fonts/fonts.spec $(TARGETDIR)/fonts
	install fonts/*.ttf $(TARGETDIR)/fonts
	install scripts/*.itcl $(TARGETDIR)/scripts
	install scripts/tclIndex $(TARGETDIR)/scripts

uninstall:
	-rm -rf $(TARGETDIR)

clean:
	-rm -rf scripts-stamp scripts/tclIndex
