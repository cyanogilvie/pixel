#!/usr/bin/itkwish3.1

set auto_path	[concat . $auto_path]

package require Pixel
package require Pixel_tkimage
package require Pixel_imlib2

set orig	[pixel::imlib2::load_image "rletest4.gif"]
set rledata	[pixel::rle_encode $orig]
puts "rledata size: ([string length $rledata])"
set fp	[open "rletest.rle" w]
fconfigure $fp -translation binary -encoding binary
puts -nonewline $fp $rledata
close $fp
set test	[pixel::rle_decode $rledata]

image create pmap before -pmap $orig
image create pmap after -pmap $test

label .before -image before
label .after -image after

pack .before .after -side left


