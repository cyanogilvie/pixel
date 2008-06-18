#!/bin/sh -x

make distclean
autoconf
./configure --prefix=/home/cyan/git/pristine8.5 --with-tcl=/home/cyan/git/pristine8.5/lib --build=linux --host=i586-mingw32msvc --target=i586-mingw32msvc
make || exit 1
make genstubs || exit 1
make binaries || exit 1

cp libPixelstub3.a /home/cyan/git/pristine8.5/lib
echo "Meta platform   win32-ix86" >> teapot.txt
teapot-pkg generate --type zip --output teapot .
cp generic/pixel.h /home/cyan/git/pristine8.5/include/Pixel
cp generic/pixelDecls.h /home/cyan/git/pristine8.5/include/Pixel
#make distclean
