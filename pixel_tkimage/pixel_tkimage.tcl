package require Pixel

load [file join $::pixel::tkimage::library pixel_tkimage.so]

namespace eval ::pixel::tkimage {
	namespace export *

	variable version 1.0
}

lappend auto_path [file join $::pixel::tkimage::library scripts]

package provide Pixel_tkimage $::pixel::tkimage::version

