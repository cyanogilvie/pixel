#!/usr/bin/itkwish3.1

set auto_path [concat . $auto_path]

package require Pixel_sdl
package require Pixel_tkimage
package require Pixel_jpeg

set img	[pixel::jpeg::loadjpeg "test.jpg"]
foreach {width height} [pixel::pmap_info $img] break

if {0} {
	image create pmap testimg -width $width -height $height

	set scrpmap	[testimg pmap]
	pixel::pmap_paste $scrpmap $img 0 0 0

	label .l -image testimg
	pack .l

	after 1000 {testimg do_frame}
	after 2000 {pixel::tkimage::do_frame $img}
} else {
	image create pmap testimg -pmap $img

	label .l -image testimg
	pack .l

	after 1000 {testimg do_frame}
}
