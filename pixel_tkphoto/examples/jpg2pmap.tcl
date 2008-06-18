#!/usr/bin/env tclsh8.5

package require Tcl 8.5
package require Img
package require Pixel_tkphoto

image create photo orig -file [file join [file dirname [info script]] sample.jpg]
set copy_pmap	[pixel::tkphoto::image2pmap orig]
image create photo copy -format "pmap" -data $copy_pmap

label .l1 -image orig
label .l2 -image copy

pack .l1 .l2 -side left
