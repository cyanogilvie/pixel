package require Pixel

load [file join $::pixel::imlib2::library pixel_imlib2.so]

namespace eval ::pixel::imlib2 {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::imlib2::library scripts]

package provide Pixel_imlib2 $::pixel::imlib2::version

