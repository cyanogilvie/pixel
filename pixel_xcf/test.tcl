#!/usr/bin/wish8.3

package require Pixel
package require Pixel_xcf
package require Pixel_ptc

set filename	"test.xcf"
set img		[pixel::xcf::loadxcf $filename]
foreach {width height} [pixel::pmap_info $img] break

set handle	[pixel::xcf::open_file $filename]
pixel::xcf::image_get_size $handle width2 height2
puts "pixel::xcf::image_get_size: width: ($width2) height: ($height2)"

set nlayers	[pixel::xcf::image_get_num_layers $handle]
puts "pixel::xcf::image_get_num_layers: ($nlayers)"

set bpp	[pixel::xcf::image_get_bpp $handle]
puts "pixel::xcf::image_get_bpp: ($bpp)"

set rowstride [pixel::xcf::image_get_rowstride $handle]
puts "pixel::xcf::image_get_rowstride: ($rowstride)"

for {set i 0} {$i < $nlayers} {incr i} {
    puts ""
    set layer($i,name)  [pixel::xcf::layer_get_name $handle $i]
	if {$layer($i,name) == "Background"} {
		#set limg	[pixel::xcf::layer_render $handle $i]
		set limg	[pixel::xcf::layer_render $handle 1]
		set limg_i	$i
		foreach {limg_x limg_y} [pixel::pmap_info $limg] break
	}
    pixel::xcf::layer_get_size $handle $i layer($i,width) layer($i,height)
    pixel::xcf::layer_get_offset $handle $i layer($i,ofs_x) layer($i,ofs_y)
    set layer($i,bpp)   [pixel::xcf::layer_get_bpp $handle $i]
    puts "layer name:   ($layer($i,name))"
    puts "layer size:   width: ($layer($i,width)) height: ($layer($i,height))"
    puts "layer offset: x: ($layer($i,ofs_x)) y: ($layer($i,ofs_y))"
    puts "layer bpp:    ($layer($i,bpp))"
}
puts ""

set img2	[pixel::xcf::image_render $handle]

pixel::xcf::close_file $handle

set img3		[pixel::xcf::loadxcf $filename infolist]
puts "\ninfolist: ($infolist)"

pixel::ptc::pixel_ptc .foo -width $width -height $height
pixel::ptc::pixel_ptc .bar -width $width2 -height $height2
pixel::ptc::pixel_ptc .baz -width [lindex $infolist 0] -height [lindex $infolist 1]
#pixel::ptc::pixel_ptc .quux -width $layer($limg_i,width) -height $layer($limg_i,height)
pixel::ptc::pixel_ptc .quux -width $limg_x -height $limg_y
set console		[.foo pmap]
set console2	[.bar pmap]
set console3	[.baz pmap]
set console4	[.quux pmap]

pixel::pmap_paste $console $img 0 0 0
pixel::pmap_paste $console2 $img2 0 0 0
pixel::pmap_paste $console3 $img3 0 0 0
pixel::pmap_paste $console4 $limg 0 0 0

#pack .foo .bar .baz .quux -side left -padx 4
pack .foo .quux -side left -padx 4

pixel::ptc::do_frame $::console
pixel::ptc::do_frame $::console2
pixel::ptc::do_frame $::console3
pixel::ptc::do_frame $::console4

puts "preview: ([pixel::xcf::layer_inf preview $infolist])"

