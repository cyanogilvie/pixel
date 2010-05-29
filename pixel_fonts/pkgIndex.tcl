#Tcl package index file, version 1.2

package ifneeded Pixel_fonts 0.2 [list apply {
	{dir} {
		source [file join $dir fonts.tcl]

		namespace eval ::pixel::fonts {
			namespace export *

			variable library
			variable version 0.2

			package provide Pixel_fonts $version
		}

		set ::pixel::fonts::library $dir
	}
} $dir]

