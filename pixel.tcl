if {$tcl_platform(platform) == "windows"} {
	load [file join $::pixel::library libpixel.dll]
} else {
	load [file join $::pixel::library libpixel.so]
}

namespace eval ::pixel {
	namespace export *

	variable version 2.2

	namespace eval :: {
		# effects flags
		variable MD_BLUR			0x00000001
		variable MD_CHANNEL_MASK	0x00000002
		# filter flags
		variable MD_FILTER_R		0x00000100
		variable MD_FILTER_G		0x00000200
		variable MD_FILTER_B		0x00000400
		variable MD_FILTER_A		0x00000800
		variable MD_FILTER_SMOOTH	0x00001000
		variable MD_FILTER_ALPHA	0x00002000
		variable MD_FILTER_ALPHA_Q	0x00004000
		# blitter flags
		variable MD_BLEND			0x00010000
		variable MD_BLIT			0x00020000
		variable MD_ALPHA			0x00040000
		variable MD_ALPHA_UNDER		0x00080000
		variable MD_ADDITIVE		0x00100000
		# scale lookup table flags
		variable MD_SCALE_SQUARE	0x01000000
		variable MD_SCALE_SINE		0x02000000
		variable MD_SCALE_LINEAR	0x04000000
		# channel masks
		variable MD_MASK_ALPHA		0xff000000
		variable MD_MASK_RED		0x00ff0000
		variable MD_MASK_GREEN		0x0000ff00
		variable MD_MASK_BLUE		0x000000ff
	}
}

lappend auto_path [file join $::pixel::library scripts]

package provide Pixel $::pixel::version

