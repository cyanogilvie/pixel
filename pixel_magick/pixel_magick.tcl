package require Pixel

if {$::tcl_platform(platform) == "windows"} {
	load [file join $::pixel::magick::library pixel_magick.dll]
} else {
	load [file join $::pixel::magick::library pixel_magick.so]
}

namespace eval ::pixel::magick {
	namespace export *

	variable version 1.0
}

#lappend auto_path [file join $::pixel::magick::library scripts]

package provide Pixel_magick $::pixel::magick::version

