package require Itcl 3.1

package require Pixel

namespace eval ::pixel::utils {
	namespace export *
	
	variable version	0.1
}

lappend auto_path [file join $::pixel::utils::library scripts]

package provide Pixel_utils $::pixel::utils::version


# For now we need to import all of the itcl functions into the global
# namespace. This should be removed once iwidgets are upgraded to use the
# itcl:: names directly.

namespace import -force itcl::*


