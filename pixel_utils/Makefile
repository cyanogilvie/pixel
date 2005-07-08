DESTDIR=

MYDESTDIR=$(DESTDIR)/usr/lib/pixel_utils

all: scripts

scripts: scripts-stamp

scripts-stamp: scripts/*.itcl
	./make_tclIndex.tcl
	touch scripts-stamp

install: all
	install -d $(MYDESTDIR)
	install -d $(MYDESTDIR)/scripts
	install pkgIndex.tcl $(MYDESTDIR)
	install utils.tcl $(MYDESTDIR)
	install scripts/*.itcl $(MYDESTDIR)/scripts
	install scripts/tclIndex $(MYDESTDIR)/scripts

uninstall:
	-rm -rf $(MYDESTDIR)

clean:
	-rm -rf scripts/tclIndex scripts-stamp
