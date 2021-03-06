#!/usr/bin/tclsh8.3

set auto_path	[concat . $auto_path]

package require ML
package require Pixel_sdl
package require Pixel_magick

set xres	800
set yres	600

proc progress {percent} {
	puts "progress: ($percent)"
}

set scr	[pixel::sdl::setup_screen $xres $yres 32]
set bg	[pixel::magick::load_image [file join /src ucss lyrics images "default_blue.jpg"] progress]
#set bg	[pixel::magick::load_image [file join .. "bg.tiff"]]
#set scaled	[pixel::magick::scale_pmap $bg $xres $yres 0 100 100 100 100 progress]
set scaled	[pixel::magick::scale_pmap $bg $xres $yres]
#pixel::pmap_paste $scr $bg 0 0 0
pixel::pmap_paste $scr $scaled 0 0 0

#pixel::magick::save_image $scr "test_save.jpg" "jpg" progress
#pixel::magick::save_image $scr "test_save.png"

proc expose {evname sdl_event} {
	pixel::sdl::do_frame $::scr
}

proc key {evname sdl_event} {
	array set ev $sdl_event

	if {$ev(state) == "SDL_PRESSED"} {
		if {$ev(ascii) == "q" || $ev(sym) == 27} {
			exit
		}
		if {$ev(ascii) == "f"} {
			pixel::sdl::toggle_fullscreen $::scr
		}
	}
}

proc quit {evname sdl_event} {
	exit
}

proc poll {clientdata} {
	pixel::sdl::dispatch_events
	update
}

pixel::sdl::bind_events expose expose
pixel::sdl::bind_events key key
pixel::sdl::bind_events quit quit

pixel::sdl::do_frame $::scr
ml::register_cb poll "" poll
ml::enter_mainloop
