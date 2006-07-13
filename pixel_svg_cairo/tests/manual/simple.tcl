#!/usr/bin/env tclsh8.4

package require TLC-base
package require Pixel_svg_cairo
package require Pixel_sdl

set fn		[lindex $argv 0]

#set pmap	[pixel::svg_cairo::load_svg $fn 300 300]
set pmap	[pixel::svg_cairo::load_svg $fn]

foreach {w h} [pixel::pmap_info $pmap] break

puts "loaded svg, w: ($w) h: ($h)"

set scr	[pixel::sdl::setup_screen $w $h 32 {SDL_ANYFORMAT SDL_RESIZABLE}]

pixel::box $scr 0 0 $w $h 0xff0000ff 0
pixel::pmap_paste $scr $pmap 0 0 0
pixel::sdl::do_frame $::scr

proc quit {evname sdl_event} {
	exit
}

proc expose {evname sdl_event} {
	pixel::sdl::do_frame $::scr
}

proc resize {evname sdl_event} {
	pixel::sdl::do_frame $::scr
}

proc key {evname sdl_event} {
	array set ev $sdl_event
	if {$ev(state) == "SDL_PRESSED"} {
		if {$ev(ascii) == "q" || $ev(sym) == 27} {
			exit
		}
	}
}

pixel::sdl::bind_events key key
pixel::sdl::bind_events expose expose
pixel::sdl::bind_events resize resize
pixel::sdl::bind_events quit quit

proc poll {} {
	pixel::sdl::dispatch_events
	after idle poll
}
poll

tlc::fake_mainloop
