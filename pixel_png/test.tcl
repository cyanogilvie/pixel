#!/usr/bin/itkwish3.1

set auto_path	[concat . /usr/lib/tclpkgs]

package require Pixel
package require Pixel_png
package require Pixel_tkimage

set filename	"testpat.png"
puts "dimensions: ([pixel::png::png_dimensions $filename])"
puts "is_png(test.png): ([pixel::png::is_png test.png])"
puts "is_png(test.gif): ([pixel::png::is_png test.gif])"
set img		[pixel::png::loadpng $filename]
foreach {w h} [pixel::pmap_info $img] break

set bg		[pixel::pmap_new $w $h 0]
pixel::box $bg 0 0 $w $h 0xff7f7f7f 0
pixel::pmap_paste $bg $img 0 0 $::MD_ALPHA

set tkimage	[image create pmap -pmap $bg]

label .l -image $tkimage

pack .l

