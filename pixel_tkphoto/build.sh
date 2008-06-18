#!/bin/sh

./build_linux.sh || exit 1
./build_win32.sh || exit 1

teacup remove Pixel_tkphoto
teacup install teapot/package-Pixel_tkphoto-1.1.0-linux-glibc2.6-ix86.zip
teacup install teapot/package-Pixel_tkphoto-1.1.0-win32-ix86.zip
