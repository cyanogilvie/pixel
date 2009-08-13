package require Pixel

namespace eval ::pixel::fonts {
	namespace export *

	variable library \
			[file normalize [file join [pwd] [file dirname [info script]]]]

	variable version 0.1
	set hold	[pwd]
	cd $::pixel::fonts::library
	set library	[pwd]
	cd $hold
}

lappend auto_path [file join $::pixel::fonts::library scripts]

package provide Pixel_fonts $::pixel::fonts::version


