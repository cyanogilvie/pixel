package require Pixel

if {$::tcl_platform(platform) == "windows"} {
	load [file join $::pixel::hermes::library pixel_hermes.dll]
} else {
	load [file join $::pixel::hermes::library pixel_hermes.so]
}

namespace eval ::pixel::hermes {
	namespace export *

	variable version 1.0
}

package provide Pixel_hermes $::pixel::hermes::version

