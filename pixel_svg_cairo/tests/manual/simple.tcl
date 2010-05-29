# vim: ft=tcl foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4

#lappend auto_path	[file normalize [file join [info script] .. .. ..]]
lappend auto_path	[file join $env(HOME) .tbuild repo pkg linux-glibc2.10-x86_64]

package require Pixel_svg_cairo
package require Pixel_sdl
package require cflib

set w	1600
set h	1000
#set w	300
#set h	300

if {[llength $argv] == 0} {
	puts stderr "No files specified"
	exit 2
}
set scr	[pixel::sdl::setup_screen $w $h 32 {SDL_ANYFORMAT SDL_RESIZABLE}]

pixel::box $scr 0 0 $w $h 0xffffffff 0
foreach fn $argv {
	if {![file exists $fn]} {
		puts stderr "No such file: \"$fn\""
		exit 2
	}
	set data	[cflib::readfile $fn]
	set pmap	[pixel::svg_cairo::load_svg $data $w $h]
	#set pmap	[pixel::svg_cairo::load_svg $data]
	#puts [time {
	#	set pmap	[pixel::svg_cairo::load_svg $data]
	#} 50]
	#lassign [pixel::pmap_info $pmap] w h

	puts "loaded svg \"$fn\", w: ($w) h: ($h)"

	#pixel::pmap_paste $scr $pmap 0 0 $::pixel::MD_ALPHA
	pixel::pmap_paste $scr $pmap 0 0 0
	pixel::sdl::do_frame $::scr
	after 500
}

pixel::sdl::do_frame $::scr

proc quit {evname sdl_event} {
	exit
}

proc expose {evname sdl_event} {
	pixel::sdl::do_frame $::scr
}

proc resize {evname sdl_event} {
	pixel::sdl::do_frame $::scr
}

proc key {evname sdl_event} {
	array set ev $sdl_event
	if {$ev(state) == "SDL_PRESSED"} {
		if {$ev(ascii) == "q" || $ev(sym) == 27} {
			exit
		}
	}
}

pixel::sdl::bind_events key key
pixel::sdl::bind_events expose expose
pixel::sdl::bind_events resize resize
pixel::sdl::bind_events quit quit

proc poll {} {
	pixel::sdl::dispatch_events
	after 25 poll
}
poll

vwait ::forever

