package require Pixel

load [file join $::pixel::xcf::library libpixel_xcf.so]

namespace eval ::pixel::xcf {
	namespace export *

	variable version 1.1

	proc layer_inf {layername fileinfo} {
		foreach {img_width img_height img_numlayers layers} $fileinfo break
		foreach layer $layers {
			foreach {name x y width height} $layer break
			if {$name == $layername} {
				return [list $x $y $width $height]
			}
		}
		return {}
	}
}

lappend auto_path [file join $::pixel::xcf::library scripts]

package provide Pixel_xcf $::pixel::xcf::version

