#!/test/bin/tclsh8.4

set auto_path [concat . $auto_path]

package require Pixel_ptc
package require Pixel_tiff
namespace import ::pixel::ptc::*
namespace import ::pixel::tiff::*

set ::mouse_x	0
set ::mouse_y	0
proc mouse_handler {x y state} {
#	puts "mouse_handler: ($x) ($y) ($state)"
	set ::mouse_x	$x
	set ::mouse_y	$y
}


proc kbd_handler {kcode} {
#	puts "kbd_handler: ($kcode)"
}


set ::opacity_degs	0
proc idle_handler {} {
#	puts "idle handler!"
	update

	set ::opacity_degs	[expr {
		$::opacity_degs + (270 * $mdgfx::elapsed / 1000000.0)
	}]
	while {$::opacity_degs > 360} {
		set ::opacity_degs	[expr {$::opacity_degs - 360}]
	}
	set factor	[expr {sin($::opacity_degs * 0.01745329252) * 0.5 + 0.5}]
	
	buf_clr $::render_buf
#	buf_displace $::render_buf -2 -2
#	buf_filter $::render_buf $::MD_FILTER_ALPHA_Q 0.9

	buf_cpy $::bg_buf $::MD_DRAW_BUF 0
#	pmap_cpy $::mouse_x $::mouse_y $::block $::MD_DRAW_BUF $::MD_BLEND
#	pmap_cpy $::mouse_x $::mouse_y $::block $::render_buf 0
	pmap_cpy $::mouse_x $::mouse_y $::block $::render2_buf 0
	buf_scale_linear $::render2_buf $::render_buf [expr {$factor * 0.5 + 1}]
	buf_filter $::render_buf $::MD_FILTER_ALPHA_Q $factor
	buf_cpy $::render_buf $::MD_DRAW_BUF $::MD_BLEND
#	buf_cpy $::render_buf $::MD_DRAW_BUF $::MD_ADDITIVE
	
	do_frame
	puts "elapsed1: ([elapsed_time])  elapsed2: ($mdgfx::elapsed)  frames: ($mdgfx::frames)  fps: ($mdgfx::fps)"
}


#setup_gfx 640 480
setup_gfx 380 400
#setup_gfx 320 200

set bg_buf		[new_buf]
set render_buf	[new_buf]
set render2_buf	[new_buf]

set bg		[loadtiff [file join $::mdgfx::library bg.tiff]]
set block	[loadtiff [file join $::mdgfx::library block.tiff]]

pmap_cpy 0 0 $bg $bg_buf 0
buf_cpy $bg_buf $::MD_DRAW_BUF 0
do_frame

register_mouse_handler mouse_handler
register_kbd_handler kbd_handler
register_idle_handler idle_handler

main_loop

