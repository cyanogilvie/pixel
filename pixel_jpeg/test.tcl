#!/test/bin/tclsh8.4

package require Pixel
package require Pixel_jpeg
package require Pixel_ptc
package require ML

set filename	"test.jpg"
set img		[pixel::jpeg::loadjpeg $filename]
foreach {width height} [pixel::pmap_info $img] break

set console	[pixel::ptc::open_console $width $height $filename]

pixel::pmap_paste $console $img 0 0 0
pixel::line_aa_osa 0 0 [expr {$width - 0.0001}] [expr {$height - 0.0001}] \
		0xffff0000 3 $console

pixel::jpeg::savejpeg "out.jpg" $console 75

pixel::line_aa_osa 0 [expr {$height - 0.0001}] [expr {$width - 0.0001}] 0 \
		0xff00ff00 3 $console

set jpeg_data	[pixel::jpeg::encodejpeg $console 75]
set fp			[open "out2.jpg" w]
fconfigure $fp -translation binary
puts -nonewline $fp $jpeg_data
close $fp

pixel::draw_box $console 10 10 100 100 0xff00ffff 0

set img2		[pixel::jpeg::decodejpeg $jpeg_data]
pixel::pmap_paste $console $img2 0 0 0

proc idle {clientdata} {
	update
	pixel::ptc::do_frame $::console
}

ml::register_cb idle "" idle
ml::enter_mainloop

