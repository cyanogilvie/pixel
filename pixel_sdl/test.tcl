#!/usr/bin/tclsh8.3

set auto_path [concat . $auto_path]

package require Pixel_sdl
package require Pixel_tiff
package require ML
namespace import ::pixel::*
#namespace import ::pixel::sdl::*
namespace import ::pixel::tiff::*

#set scr	[pixel::sdl::setup_screen 512 384 32 {SDL_ANYFORMAT SDL_RESIZABLE}]
set scr	[pixel::sdl::setup_screen 512 384 32 {SDL_ANYFORMAT SDL_FULLSCREEN SDL_RESIZABLE}]

pixel::sdl::show_cursor 1

puts "caps:\n[join [pixel::sdl::get_caps $scr] \n]"

#set bg		[loadtiff [file join .. bg.tiff]]
#set block	[loadtiff [file join .. block.tiff]]

#pmap_paste $scr $bg 0 0 0
pixel::sdl::do_frame $::scr

proc poll {clientdata} {
	pixel::sdl::dispatch_events
	update
}

proc quit {evname sdl_event} {
	puts "got ($evname) event: ($sdl_event)"
	exit
}

proc expose {evname sdl_event} {
	# seems to be being handled further up
	puts "got ($evname) event: ($sdl_event)"
	pixel::sdl::do_frame $::scr
}

proc resize {evname sdl_event} {
	puts "got ($evname) event: ($sdl_event)"
	pixel::sdl::do_frame $::scr
}

proc general {evname sdl_event} {
	puts "got ($evname) event: ($sdl_event)"
}

proc key {evname sdl_event} {
	puts "got ($evname) event: ($sdl_event)"
	array set ev $sdl_event
	if {$ev(state) == "SDL_PRESSED"} {
		if {$ev(ascii) == "q" || $ev(sym) == 27} {
			exit
		} elseif {$ev(ascii) == "f"} {
			set stat	[pixel::sdl::toggle_fullscreen $::scr]
			puts "toggle fullscreen: status ($stat)"
		}
	}
}

proc joystick {evname sdl_event} {
	array set ev $sdl_event
	puts "got ($evname)"
	parray ev
}


pixel::sdl::bind_events key key
pixel::sdl::bind_events button general
pixel::sdl::bind_events expose expose
pixel::sdl::bind_events resize resize
pixel::sdl::bind_events jaxis joystick
pixel::sdl::bind_events jball joystick
pixel::sdl::bind_events jhat joystick
pixel::sdl::bind_events jbutton joystick
pixel::sdl::bind_events quit quit

puts "Found [pixel::sdl::numjoysticks] joysticks"

for {set i 0} {$i < [pixel::sdl::numjoysticks]} {incr i} {
	puts "  Joystick #$i: ([pixel::sdl::joystickname $i])"
	catch {unset jsinfo}
	array set jsinfo	[pixel::sdl::joystickopen $i]
	parray jsinfo
}

ml::register_cb poll "" poll
ml::enter_mainloop

