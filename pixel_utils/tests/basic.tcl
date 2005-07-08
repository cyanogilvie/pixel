#!/usr/bin/tclsh

set auto_path	[concat [list ..] $auto_path]

package require Tk
package require Pixel_utils
package require Pixel_sdl
package require Pixel_tkimage

pixel::utils::Graph g \
		-width 60 \
		-height 50 \
		-bg_colour "0xffd5dad5" \
		-linestyle "simple" \
		-auto_ticks_x 1 \
		-auto_ticks_y 1 \
		-prec_y 2

array set series {
	used		{}
	buffers		{}
	cached		{}
	swap_used	{}
}

array set series_colours {
	used		0xffff0000
	buffers		0xffff7000
	cached		0xffffc000
	swap_used	0xff0000ff
}

proc get_mem_usage {} {
	if {[catch {
		set dat	[exec free]
	} errmsg]} {
		puts stderr "Error getting mem info: $errmsg"
		return {}
	}

	array set mem	{}
	set lines	[split $dat \n]
	set mem(used)		[lindex [lindex $lines 2] 2]
	set mem(buffers)	[lindex [lindex $lines 1] 5]
	set mem(cached)		[lindex [lindex $lines 1] 6]
	set mem(swap_used)	[lindex [lindex $lines 3] 2]
	set mem(swap_free)	[lindex [lindex $lines 3] 3]

	return [array get mem]
}

proc render_update {} {
	array set mem	[get_mem_usage]

	foreach key {used buffers cached swap_used} {
		lappend ::series($key) \
				[expr {[clock seconds] % 60}] \
				[expr {$mem($key) / 1024.0}]
		set ::series($key)		[lrange $::series($key) end-80 end]
	}

	g x_axis [list "0" 0]
	g y_axis [list "0" 0]

	if {[llength $::series(used)] > 10} {
		g x_label "Seconds"
		g y_label "Mem used"

		foreach key {used buffers cached swap_used} {
			puts stderr "plotting: ($::series($key))"
			g data $key $::series_colours($key) $::series($key)
		}

		g plot

		.l configure -image [g get_image]
	}

	after 1000 render_update
}


label .l
pack .l

render_update
