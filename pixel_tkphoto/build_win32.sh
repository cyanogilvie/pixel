#!/bin/sh -x

make distclean
autoconf
./configure --prefix=/home/cyan/git/pristine8.5 --with-tcl=/home/cyan/git/pristine8.5/lib --build=linux --host=i586-mingw32msvc --target=i586-mingw32msvc
make || exit 1
echo "Meta platform   win32-ix86" >> teapot.txt
teapot-pkg generate --type zip --output teapot .
#make distclean
exit 0
