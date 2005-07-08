#!/usr/bin/wish

set auto_path	[concat [list ..] $auto_path]

package require Pixel_utils
package require Pixel_imlib2

label .l

pixel::utils::Graph g \
		-label "" \
		-width 400 \
		-height 300 \
		-linestyle "aa_osa" \
		-bg_mode "place -10 20"
#		-bg_mode "tile"

g x_label "Service Level"
g y_label "Profit"

g x_axis {
	0.000	"0.000"
	0.011	"0.011"
	0.022	"0.022"
	0.033	"0.033"
	0.044	"0.044"
	0.055	"0.055"
}

g y_axis {
	-0.2	"-0.2"
	0		"0.0"
	1		"1.0"
	2		"2.0"
	3		"3.0"
	3.5		"3.5"
}

g data "Foo" 0xffff0000 {
	0		1.0
	0.005	2.1
	0.02	1.95
	0.042	1.5
	0.0435	-0.2
	0.06	1.2
}

g data "Bar" 0xff0000ff {
	0		0.5
	0.011	2.0
	0.012	3.5
	0.013	1.5
	0.05	1.2
	0.06	1.4
}

set sinepoints	{}
for {set i 0} {$i < 0.058} {set i	[expr {$i + 0.0003}]} {
	lappend sinepoints	$i	[expr {sin($i * 1000) * (sin($i * 410) * 0.7) + 1.05 + sin($i * 60 - 0.5) * 0.9 + rand()*0.5 - 0.25}]
}

g data "Baz" 0xff00ff00 $sinepoints

set sinepoints2	{}
for {set i 0} {$i < 0.058} {set i	[expr {$i + 0.0003}]} {
	lappend sinepoints2	$i	[expr {sin($i * 60 - 0.5) * 0.9 + 1.05}]
}

g data "Quux" 0xffff00ff $sinepoints2

g bg_pmap [pixel::imlib2::load_image "dnalogo-topleft.png"]

g plot
.l configure -image [g get_image]
g write_image "fooseries.png"

pack .l


