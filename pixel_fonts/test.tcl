#!/usr/bin/itkwish3.1

set auto_path	[concat . $auto_path]

package require TLC
namespace import tlc::*
eval [go_home]

proc bgerror {args} {
	puts stderr $::errorInfo
}

package require Pixel_tkimage
package require Pixel_fonts

pixel::fonts::Font fontreg

class Main {
	inherit tlc::Application

	constructor {args} {
		set armed	1
		eval itk_initialize $args

		Form $w.details -schema [list \
			"Font"		[list face mycombobox -choices [fontreg list_basenames] -cb [code $this update_attribs]] \
			"Attribs"	[list attribs mycombobox -choices {}] \
			"Size"		[list size] \
			"Text"		[list text entry -width 50] \
		]
		$w.details set_data {
			face		"arial"
			size		14
			text		"Hello World!"
		}

		set pmap	[pixel::pmap_new 400 60 0xffffffff]
		pixel::box $pmap 0 0 400 60 0xffffffff 0

		set image	[image create pmap -pmap $pmap]

		label $w.sample -image $image -borderwidth 2 -relief sunken

		Tools $w.tools
		$w.tools add "Update" [code $this rerender] right
		$w.tools add "Close" [code $this closewin] right
		
		table $w -padx 5 -pady 5 \
				$w.details		1,1 \
				$w.sample		2,1 -pady 8 \
				$w.tools		3,1 -fill x

		set armed	0
	}

	private {
		variable pmap
		variable image
		
		method update_attribs {basename} {
			$w.details itemconfig "Attribs" -choices [fontreg list_attribs $basename]
		}

		method rerender {} {
			array set formdat	[$w.details get_data]
			pixel::box $pmap 0 0 400 60 0xffffffff 0
			set tmp		[pixel::render_ttf 0xff000000 [fontreg get_face [list $formdat(face) $formdat(attribs)]] $formdat(size) $formdat(text)]
			pixel::pmap_paste $pmap $tmp 3 3 $::MD_ALPHA
			$image do_frame
		}
	}
}


Main .main -title "Test font registry"
.main show
