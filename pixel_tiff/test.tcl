#!/test/bin/tclsh8.4

package require Pixel
package require Pixel_tiff
package require Pixel_ptc
package require ML

set filename	"test.tiff"
set img		[pixel::tiff::loadtiff $filename]
foreach {width height} [pixel::pmap_info $img] break

set console	[pixel::ptc::open_console $width $height $filename]

pixel::pmap_paste $console $img 0 0 0
pixel::line_aa_osa 0 0 [expr {$width - 0.0001}] [expr {$height - 0.0001}] \
		0xffff0000 3 $console

pixel::tiff::savetiff "out.tiff" $console

proc idle {clientdata} {
	update
	pixel::ptc::do_frame $::console
}

ml::register_cb idle "" idle
ml::enter_mainloop

