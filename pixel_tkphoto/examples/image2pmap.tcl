#!/usr/bin/env tclsh8.5

package require Tcl 8.5
package require Pixel_tkphoto

set pmap	[pixel::pmap_new 100 100 0x0]
pixel::box $pmap 0 0 100 100 0xff000000 0
pixel::box $pmap 5 5 10 10 0xffff0000 0
pixel::box $pmap 20 5 10 10 0xff00ff00 0
pixel::box $pmap 35 5 10 10 0xff0000ff 0
pixel::box $pmap 50 5 10 10 0x00000000 0

#image create photo orig -format "pmap" -data $pmap
image create photo orig
pixel::tkphoto::violate_photo orig $pmap
set copy_pmap	[pixel::tkphoto::image2pmap orig]
#image create photo copy -format "pmap" -data $copy_pmap
image create photo copy
pixel::tkphoto::violate_photo copy $copy_pmap

label .l1 -image orig
label .l2 -image copy

pack .l1 .l2 -side left
