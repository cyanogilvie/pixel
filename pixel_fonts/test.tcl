#!/tcl8.4/bin/tclsh8.4

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
fontreg add_ttf_files [glob -nocomplain -type f [file join extras *.ttf]]
fontreg add_ttf_files [glob -nocomplain -type f [file join extras *.TTF]]

puts "basenames:\n[join [fontreg list_basenames] \n]"

class Main {
	inherit tlc::Application

	constructor {args} {
		set armed	1
		eval itk_initialize $args

		Form $w.details -schema [list \
			"Font"		[list face mycombobox -choices [fontreg list_basenames] -cb [code $this update_attribs]] \
			"Attribs"	[list attribs mycombobox -choices {}] \
			"Size"		[list size] \
			"Text"		[list text vartextbox -width 50 -height 2] \
		]
		$w.details set_data {
			face		"arial"
			size		14
			text		"Hello World!"
		}

		set pmap	[pixel::pmap_new $samplewidth 60 0xffffffff]
		#pixel::box $pmap 0 0 $samplewidth 60 0xffffffff 0
		pixel::box $pmap 0 0 $samplewidth 60 0xff000000 0

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
		variable samplewidth	400
		
		method update_attribs {basename} {
			$w.details itemconfig "Attribs" -choices [fontreg list_attribs $basename]
		}

		method rerender {} {
			array set formdat	[$w.details get_data]
			#pixel::box $pmap 0 0 $samplewidth 60 0xffffffff 0
			pixel::box $pmap 0 0 $samplewidth 60 0xff000000 0
			#set tmp		[pixel::render_ttf 0xff000000 [fontreg get_face [list $formdat(face) $formdat(attribs)]] $formdat(size) $formdat(text)]
			set meta(base_col)	0xfff7c500
			set meta(face)		[fontreg get_face [list $formdat(face) $formdat(attribs)]]
			set meta(px_size)	$formdat(size)
			set meta(width)		$samplewidth
			set pmaps		[pixel::render_ttf_adv meta $formdat(text)]
			set fromy		3
			foreach tmp $pmaps {charstart xofs} $meta(feedback) {
				set toy		$fromy
				incr toy	$formdat(size)
				foreach x $xofs {
					incr x	3
					#pixel::line $x $fromy $x $toy 0xffe0e0e0 $pmap
				}
				pixel::pmap_paste $pmap $tmp 3 $fromy $::MD_ALPHA
				incr toy	2
				set fromy	$toy
			}
			$image do_frame
		}
	}
}


Main .main -title "Test font registry"
.main show
