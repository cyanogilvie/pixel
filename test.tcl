#!/usr/bin/wish8.3
#!/test/bin/wish8.4

set auto_path [concat . $auto_path]

package require Itcl
namespace import -force itcl::*

package require Pixel 2.4
namespace import pixel::*
package require Pixel_ptc
namespace import pixel::ptc::*
package require Pixel_tiff
namespace import pixel::tiff::*

package require ML
namespace import ml::*

set reg		[pixel::Regions ::#auto]

set ::opacity_degs	0
proc idle_handler {clientdata} {
#	puts "idle handler!"
	update

	set ::opacity_degs	[expr {
		$::opacity_degs + (270 * [pixel::ptc::elapsed $::scr_pmap] / 1000000.0)
	}]
	while {$::opacity_degs > 360} {
		set ::opacity_degs	[expr {$::opacity_degs - 360}]
	}
	set factor	[expr {sin($::opacity_degs * 0.01745329252) * 0.5 + 0.5}]
	
	if {1} {
		set cx	189
		set cy	199
		# 190, 200

#		buf_clr $::render_buf
#		buf_clr $::render2_buf
#		buf_cpy $::bg_buf $::scr_pmap $::MD_BLIT
		pmap_paste $::scr_pmap $::bg 0 0 0

#		box $::scr_pmap 0 0 379 399 0xffffffff
		box $::scr_pmap \
				[expr {$cx - 99}] \
				[expr {$cy - 99}] \
				200 \
				200 \
				0xffffffff 0

		set fact	10
		set diff	12
		set len		4
#		line_aa \
#				[expr {$cx - ($len)}] \
#				[expr {$cy - ($len + $diff)}] \
#				[expr {$cx + $len}] \
#				[expr {$cy + $len + $diff}] \
#				0xff00ff00 $::render2_buf
#		line_aa \
#				[expr {$cx - ($len + $diff)}] \
#				[expr {$cy - $len}] \
#				[expr {$cx + $len + $diff}] \
#				[expr {$cy + $len}] \
#				0xff00ff00 $::render2_buf
##		line_aa $cx $cy $cx $cy 0xffff0000 $::render2_buf

#		buf_scale_square $::render2_buf $::render_buf $fact

#		buf_cpy $::render_buf $::scr_pmap $::MD_ALPHA_UNDER
		#	buf_cpy $::render_buf $::scr_pmap 0
		line_aa_osa \
				[expr {$cx - 100 + 0.5}] \
				[expr {$cy + 0.5}] \
				[expr {$cx + 100 + 0.5}] \
				[expr {$cy + 0.5}] \
				0xff00ff00 3 $::scr_pmap

		line_aa_osa \
				[expr {$cx + 0.5}] \
				[expr {$cy - 100 + 0.5}] \
				[expr {$cx + 0.5}] \
				[expr {$cy + 100 + 0.5}] \
				0xff00ff00 3 $::scr_pmap

		line_aa_osa \
				[expr {($cx - 100) + 0.0}] \
				[expr {($cy + 100) + 0.999}] \
				[expr {($cx + 100) + 0.999}] \
				[expr {($cy - 100) + 0.0}] \
				0xffffff00 3 $::scr_pmap

		# Box <<<
		line_aa_osa \
				[expr {($cx - 100) + 0.5}] \
				[expr {($cy - 100) + 0.5}] \
				[expr {($cx + 100) + 0.5}] \
				[expr {($cy - 100) + 0.5}] \
				0xefdfdfdf 3 $::scr_pmap
		line_aa_osa \
				[expr {($cx + 100) + 0.5}] \
				[expr {($cy + 100) + 0.5}] \
				[expr {($cx + 100) + 0.5}] \
				[expr {($cy - 100) + 0.5}] \
				0xefdfdfdf 3 $::scr_pmap
		line_aa_osa \
				[expr {($cx - 100) + 0.5}] \
				[expr {($cy + 100) + 0.5}] \
				[expr {($cx - 100) + 0.5}] \
				[expr {($cy - 100) + 0.5}] \
				0xefdfdfdf 3 $::scr_pmap
		line_aa_osa \
				[expr {($cx - 100) + 0.5}] \
				[expr {($cy + 100) + 0.5}] \
				[expr {($cx + 100) + 0.5}] \
				[expr {($cy + 100) + 0.5}] \
				0xefdfdfdf 3 $::scr_pmap
		# Box >>>

		line_aa \
				100 100 250 250 \
				0xff0000ff $::scr_pmap

		line_aa_osa \
				95.0 105.0 245.999 255.999 \
				0xff0000ff 3 $::scr_pmap

		line_aa_osa \
				90.5 110.5 240.5 260.5 \
				0xff0000ff 3 $::scr_pmap

		
		line_aa_osa \
				240.5 260.5 250.5 250.5  \
				0xff0000ff 3 $::scr_pmap

#		line_aa_osa \
#				[expr {240.999-$factor}] \
#				[expr {260.999-$factor}] 250.5 250.0  \
#				0xff0000ff 3 $::scr_pmap


		line_aa_osa \
				[expr {$cx - ($len + $diff)*$fact + 0.5}] \
				[expr {$cy - ($len)*$fact + 0.5 + 10}] \
				[expr {$cx + ($len + $diff)*$fact + 0.5}] \
				[expr {$cy + ($len)*$fact + 0.5 + 10}] \
				0xffff0000 3 $::scr_pmap

		line_aa_osa \
				[expr {$cx - ($len)*$fact + 0.5 + 10}] \
				[expr {$cy - ($len + $diff)*$fact + 0.5}] \
				[expr {$cx + ($len)*$fact + 0.5 + 10}] \
				[expr {$cy + ($len + $diff)*$fact + 0.5}] \
				0xffff0000 3 $::scr_pmap

		line_aa \
				[expr {$cx - ($len)*$fact}] \
				[expr {$cy - ($len + $diff)*$fact}] \
				[expr {$cx + ($len)*$fact}] \
				[expr {$cy + ($len + $diff)*$fact}] \
				0xffff0000 $::scr_pmap
		line_aa \
				[expr {$cx - ($len + $diff)*$fact}] \
				[expr {$cy - ($len)*$fact}] \
				[expr {$cx + ($len + $diff)*$fact}] \
				[expr {$cy + ($len)*$fact}] \
				0xffff0000 $::scr_pmap
		line \
				[expr {$cx - ($len)*$fact -10}] \
				[expr {$cy - ($len + $diff)*$fact}] \
				[expr {$cx + ($len)*$fact -10}] \
				[expr {$cy + ($len + $diff)*$fact}] \
				0xffff0000 $::scr_pmap
		line \
				[expr {$cx - ($len + $diff)*$fact}] \
				[expr {$cy - ($len)*$fact -10}] \
				[expr {$cx + ($len + $diff)*$fact}] \
				[expr {$cy + ($len)*$fact -10}] \
				0xffff0000 $::scr_pmap
		line \
				[expr {$cx + ($len + $diff)*$fact}] \
				[expr {$cy + ($len)*$fact}] \
				[expr {$cx + ($len + $diff)*$fact}] \
				[expr {$cy + ($len)*$fact}] \
				0xff0000ff $::scr_pmap
		line \
				[expr {$cx + ($len + $diff)*$fact}] \
				[expr {$cy + ($len)*$fact -10}] \
				[expr {$cx + ($len + $diff)*$fact}] \
				[expr {$cy + ($len)*$fact -10}] \
				0xff0000ff $::scr_pmap

		line \
				[expr {$cx + ($len)*$fact}] \
				[expr {$cy + ($len + $diff)*$fact}] \
				[expr {$cx + ($len)*$fact}] \
				[expr {$cy + ($len + $diff)*$fact}] \
				0xff0000ff $::scr_pmap
#		line_aa $cx $cy $cx $cy 0xff0000ff $::scr_pmap

#		line 10 10 50 10 0xff00ff00 $::scr_pmap
#		line 10 100 50 100 0xff00ff00 $::scr_pmap
		
		bezier \
				10 20 \
				60 0 \
				0 120 \
				50 100 \
				0xffff0000 1 $::scr_pmap

#		puts [format "0x%08x" [expr {int($factor * 0xff) << 24}]]
		set ::face	"grunge.ttf"
		if {![info exists ::text]} {
			set ::text	[render_ttf 0xffff0000 $::face 32 "Hello World 12.4"]
		}
#		set ::text	[render_ttf [expr {0x00ff0000 + (int($factor * 0xff) << 24)}] $::face 32 "Hello World 12.4"]
		pmap_paste $::scr_pmap $::text 10 330 $::MD_ALPHA 

		# Crosshairs <<<
		if {0} {
			line_aa_osa ${::mouse_x}.5 0.5 ${::mouse_x}.5 399.5 0xdf404040 2 $::scr_pmap
			line_aa_osa 0.5 ${::mouse_y}.5 379.5 ${::mouse_y}.5 0xdf404040 2 $::scr_pmap
			if {![info exists ::xh_face]} {
				set ::xh_face	"arial.ttf"
			}
			if {1 || !$::lock} {
				set ::lock	1
				set xy	[render_ttf 0xff000000 $::xh_face 8 "$::mouse_x, $::mouse_y"]
				set tst		[dup $xy]
				set width	[lindex $tst 0]
				set height	[lindex $tst 1]
#				unset tst
#				set width	[lindex $xy 0]
#				set height	[lindex $xy 1]
#				buf_draw_box $::scr_pmap \
#						[expr {$::mouse_x + 1}] \
#						[expr {$::mouse_y - 14}] \
#						[expr {$::mouse_x + 1 + $width}] \
#						[expr {$::mouse_y - 14 + $height}] \
#						0xfff0f300

				set f	[binary format "h*" ffff00ff]
				for {set i 0} {$i < 100} {incr i} {
					append foo $f
				}

				set bar	[list 10 10 4 $foo]

				pmap_paste \
						$::scr_pmap $xy \
						[expr {$::mouse_x + 1}] \
						[expr {$::mouse_y - 14}] \
						$::MD_ALPHA
				set ::lock	0
			} else {
#				log warning "lock contention!!"
			}
		}
		# Crosshairs >>>
		
		set wpmap	[.bar pmap]
		pixel::ptc::do_frame $::scr_pmap
		pmap_paste $wpmap $::bg 0 0 0
#		pixel::ptc::do_frame $wpmap
		.bar update
	}

	if {0} {
		buf_clr $::render_buf
		#	buf_displace $::render_buf -2 -2
		#	buf_filter $::render_buf $::MD_FILTER_ALPHA_Q 0.9

		buf_cpy $::bg_buf $::scr_pmap 0
		#	pmap_cpy $::mouse_x $::mouse_y $::block $::scr_pmap $::MD_BLEND
		#	pmap_cpy $::mouse_x $::mouse_y $::block $::render_buf 0
		pmap_cpy $::mouse_x $::mouse_y $::block $::render2_buf 0
		#render_ttf $::mouse_x $::mouse_y 0x00ff00ff "Hello World" $::render2_buf 0
		buf_scale_linear $::render2_buf $::render_buf [expr {$factor * 0.5 + 1}]
		buf_filter $::render_buf $::MD_FILTER_ALPHA_Q $factor
		buf_cpy $::render_buf $::scr_pmap $::MD_BLEND
		#	buf_cpy $::render_buf $::scr_pmap $::MD_ADDITIVE
		render_ttf 10 300 0xfffefdfc "Hello World 12.4" $::scr_pmap 0

		line_aa 30 10 260 200 0xff00ff00 $::scr_pmap
		line_aa 20 10 250 200 0xffffffff $::scr_pmap
		line 10 10 230 200 0xff00ff00 $::scr_pmap

		pixel::ptc::do_frame $::scr_pmap
	}
	puts "elapsed1: ([pixel::ptc::elapsed $::scr_pmap])  frames: ([pixel::ptc::frames $::scr_pmap])  fps: ([pixel::ptc::fps $::scr_pmap])"
}

set test	[pmap_new 10 10 0xfffefdfc]

set bg		[loadtiff [file join $::pixel::library bg.tiff]]
set block	[loadtiff [file join $::pixel::library block.tiff]]

#puts stderr "block: ($block)"
#puts stderr "block.width: ([lindex $block 0])"
#puts stderr "block.height: ([lindex $block 1])"
#puts stderr "block.bytes_per_pixel: ([lindex $block 2])"

set scr_pmap	[pixel::ptc::open_console 380 400 "Hello World"]

label .foo -text "Hello world"
pixel::ptc::pixel_ptc .bar -width 400 -height 300
#pixel::ptc::pixel_ptc .bar -width 200 -height 150 -swidth 400 -sheight 300
bind .bar <Button> {puts "button!"}
bind .bar <Key> {puts "key!"}
focus .bar

pack .foo -side top -anchor w
pack .bar -side top -anchor w -fill both -expand true

pmap_paste $scr_pmap $bg 0 0 0
box $scr_pmap 10 10 90 90 0xff80ffff 0
pixel::ptc::do_frame $scr_pmap

register_cb "prepoll" "" idle_handler 

ml::enter_mainloop

