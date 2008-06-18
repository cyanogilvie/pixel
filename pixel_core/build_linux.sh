#!/bin/sh -x

make distclean
autoconf
./configure --prefix=/tcl8.5 --with-tcl=/tcl8.5/lib --enable-symbols
/tcl8.5/bin/tclsh8.5 ~/pkgs/tcl8.5/tcl8.5.0/tools/genStubs.tcl generic generic/Pixel.decls
make || exit 1
make genstubs || exit 1
make binaries || exit 1

cp libPixelstub3.a /tcl8.5/lib
echo "Meta platform   linux-glibc2.6-ix86" >> teapot.txt
teapot-pkg generate --type zip --output teapot .
sudo cp generic/pixel.h /usr/include/Pixel
sudo cp generic/pixelDecls.h /usr/include/Pixel
#make distclean
