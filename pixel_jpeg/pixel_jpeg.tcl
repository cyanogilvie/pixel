package require Pixel

load [file join $::pixel::jpeg::library libpixel_jpeg.so]

namespace eval ::pixel::jpeg {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::jpeg::library scripts]

package provide Pixel_jpeg $::pixel::jpeg::version

