package require Pixel

load [file join $::pixel::ptc::library libpixel_ptc.so]

namespace eval ::pixel::ptc {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::ptc::library scripts]

package provide Pixel_ptc $::pixel::ptc::version

