#!/usr/bin/tclsh

set auto_path	[concat . $auto_path]

package require Pixel_tkutils

puts "was: blue now: [pixel::tkutils::colour_tk2pel blue]"

