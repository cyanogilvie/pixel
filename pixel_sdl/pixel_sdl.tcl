package require Pixel

load [file join $::pixel::sdl::library libpixel_sdl.so]

namespace eval ::pixel::sdl {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::sdl::library scripts]

package provide Pixel_sdl $::pixel::sdl::version

