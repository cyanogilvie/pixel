#!/usr/bin/itkwish3.1

set auto_path [concat . $auto_path]

#package require Pixel_sdl
package require Pixel_tkimage
package require Pixel_imlib2
package require Pixel_hermes

set img1	[pixel::imlib2::load_image "block.tiff"]
set img2	[pixel::hermes::scale_pmap $img1 10 10]
set img3	[pixel::hermes::scale_pmap $img1 100 150]
pixel::hermes::clear_pmap $img3 80 80 255 255 10 10 15 20
pixel::hermes::clear_pmap $img3 80 80 80 80 20 10 30 20

image create pmap img1 -pmap $img1
image create pmap img2 -pmap $img2
image create pmap img3 -pmap $img3

label .l1 -image img1
label .l2 -image img2
label .l3 -image img3
pack .l1 .l2 .l3 -side left

