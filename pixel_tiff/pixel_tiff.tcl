package require Pixel

if {$tcl_platform(platform) == "windows"} {
	load [file join $::pixel::tiff::library libpixel_tiff.dll]
} else {
	load [file join $::pixel::tiff::library libpixel_tiff.so]
}

namespace eval ::pixel::tiff {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::tiff::library scripts]

package provide Pixel_tiff $::pixel::tiff::version

