package require Tk
package require Pixel

if {$::tcl_platform(platform) == "windows"} {
	load [file join $::pixel::tkutils::library pixel_tkutils.dll]
} else {
	load [file join $::pixel::tkutils::library pixel_tkutils.so]
}

namespace eval ::pixel::tkutils {
	namespace export *

	variable version 0.1
}

lappend auto_path [file join $::pixel::tkutils::library scripts]

package provide Pixel_tkutils $::pixel::tkutils::version

