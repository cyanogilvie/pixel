#!/usr/bin/env cfkit8.6

# vim: ft=tcl foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4

package require Tcl 8.6
package require Pixel_sdl 1.2

set width	1024
set height	768

set scr	[pixel::sdl::setup_screen $width $height 32 {SDL_ANYFORMAT}]


proc key {evname sdl_event} { #<<<
	dict with sdl_event {}

	if {$state eq "SDL_PRESSED"} {
		if {$ascii eq "q" || $sym eq 27} {
			exit
		}
	}
}

#>>>
proc expose {evname sdl_event} { #<<<
	pixel::sdl::do_frame $::scr
}

#>>>
proc quit {evname sdl_event} { #<<<
	exit 0
}

#>>>


pixel::sdl::bind_events key key
pixel::sdl::bind_events expose expose
pixel::sdl::bind_events quit quit

proc poll {} { #<<<
	pixel::sdl::dispatch_events

	after 10 poll
}

#>>>

coroutine coro_main apply {
	{} {
		poll
		vwait ::forever
	}
}
