#!/usr/bin/env cfkit8.6

package require platform

foreach platform [platform::patterns [platform::identify]] {
	set tm_path		[file join $env(HOME) .tbuild repo tm $platform]
	set pkg_path	[file join $env(HOME) .tbuild repo pkg $platform]
	if {[file exists $tm_path]} {
		tcl::tm::path add $tm_path
	}
	if {[file exists $pkg_path]} {
		lappend auto_path $pkg_path
	}
}

set auto_path	[concat [file dirname [file dirname [file normalize [info script]]]] $auto_path]

package require Pixel 3.4.2
package require Pixel_sdl
package require Pixel_gl

namespace path [list {*}[namespace path] ::pixel::gl]

set scr		[pixel::sdl::setup_screen 800 600 32 {SDL_ANYFORMAT SDL_OPENGL}]

