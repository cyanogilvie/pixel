package require Pixel

if {$tcl_platform(platform) == "windows"} {
	load [file join $::pixel::tkimage::library pixel_tkimage.dll]
} else {
	load [file join $::pixel::tkimage::library pixel_tkimage.so]
}

namespace eval ::pixel::tkimage {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::tkimage::library scripts]

package provide Pixel_tkimage $::pixel::tkimage::version

