#!/bin/sh -x

make distclean
autoconf
./configure --prefix=/tcl8.5 --with-tcl=/tcl8.5/lib --enable-symbols
make || exit 1
echo "Meta platform   linux-glibc2.6-ix86" >> teapot.txt
teapot-pkg generate --type zip --output teapot .
#make distclean
exit 0
