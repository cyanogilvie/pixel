package require Pixel

if {$tcl_platform(platform) == "windows"} {
	load [file join $::pixel::png::library pixel_png.dll]
} else {
	load [file join $::pixel::png::library pixel_png.so]
}

namespace eval ::pixel::png {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::png::library scripts]

package provide Pixel_png $::pixel::png::version

