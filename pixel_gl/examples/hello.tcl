#!/usr/bin/env cfkit8.6

# vim: ft=tcl foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4

lappend auto_path	[file join ~ .tbuild repo pkg linux-glibc2.8-x86_64]

package require Tcl 8.6
package require Pixel_sdl 1.2
package require Pixel_gl 0.2.0

set width		1024
set height		768
set viewdist	400

set samples_fn	[lindex $argv 0]
set threshold	[lindex $argv 1]
if {$threshold == ""} {
	set threshold	0
}


set scr	[pixel::sdl::setup_screen $width $height 32 {SDL_ANYFORMAT SDL_OPENGL}]


pixel::gl::glShadeModel GL_SMOOTH
pixel::gl::glCullFace GL_BACK
pixel::gl::glFrontFace GL_CCW
pixel::gl::glEnable GL_CULL_FACE
pixel::gl::glEnable GL_LINE_SMOOTH
pixel::gl::glEnable GL_POINT_SMOOTH
pixel::gl::glClearColor 0 0 0 0
pixel::gl::glViewport 0 0 $width $height
pixel::gl::glMatrixMode GL_PROJECTION
pixel::gl::glLoadIdentity
pixel::gl::gluPerspective 60.0 [expr {$width/double($height)}] 1.0 1024.0

oo::class create Globj { #<<<
	variable {*}{
		listid
	}

	constructor {args} { #<<<
		set listid		[pixel::gl::glGenList]
	}

	#>>>

	method draw {} { #<<<
		pixel::gl::glCallList $listid
	}

	#>>>
	method start {} { #<<<
		pixel::gl::glNewList $listid
	}

	#>>>
	method end {} { #<<<
		pixel::gl::glEndList
	}

	#>>>
}


#>>>

# Draw points <<<
Globj create points
points start
set maxfreq		0
set fp	[open $samples_fn r]
while {![chan eof $fp]} {
	set line	[chan gets $fp]
	if {$line eq ""} continue
	if {[string match "*nan*" $line]} continue
	foreach {h s v f} $line break
	if {$f > $maxfreq} {
		puts "line: ($line)"
		set maxfreq	$f
	}
}
puts "maxfreq: ($maxfreq)"
set seq	0
chan close $fp
pixel::gl::glBegin GL_POINTS
pixel::gl::glPointSize 1.0
set fp	[open $samples_fn r]
while {![chan eof $fp]} {
	set line	[chan gets $fp]
	if {[string match "*nan*" $line]} continue
	if {$line eq ""} continue
	foreach {h s v f} $line break
	if {$f <= $::threshold} continue
	#set col		[expr {(double($f) / $maxfreq) * 0.7 + 0.3}]
	set col		1.0
	set val		[expr {(double($f) / $maxfreq) * 0.5 + 0.5}]
	pixel::gl::glColor4f $col $col $col $val
	#pixel::gl::glColor3f 0.0 $val 0.0
	#pixel::gl::glVertex3f [expr {$h/10}] [expr {$s/10}] [expr {$v/10}]
	pixel::gl::glVertex3f [expr {$h-180}] [expr {$s-50}] [expr {$v-50}]
	#pixel::gl::glVertex3f $h $s $v
	incr seq
}
puts "colours: ($seq)"
chan close $fp
pixel::gl::glEnd
points end
lappend objects	points
# Draw points >>>

# Draw box <<<
Globj create box
box start
pixel::gl::glBegin GL_LINES
pixel::gl::glColor4f 1.0 1.0 0.0 0.8
pixel::gl::glVertex3f -180.0 -50.0 -50.0
pixel::gl::glVertex3f 180.0 -50.0 -50.0

pixel::gl::glVertex3f 180.0 -50.0 -50.0
pixel::gl::glVertex3f 180.0 50.0 -50.0

pixel::gl::glVertex3f 180.0 50.0 -50.0
pixel::gl::glVertex3f -180.0 50.0 -50.0

pixel::gl::glVertex3f -180.0 50.0 -50.0
pixel::gl::glVertex3f -180.0 -50.0 -50.0

#-------------
pixel::gl::glVertex3f -180.0 -50.0 50.0
pixel::gl::glVertex3f 180.0 -50.0 50.0

pixel::gl::glVertex3f 180.0 -50.0 50.0
pixel::gl::glVertex3f 180.0 50.0 50.0

pixel::gl::glVertex3f 180.0 50.0 50.0
pixel::gl::glVertex3f -180.0 50.0 50.0

pixel::gl::glVertex3f -180.0 50.0 50.0
pixel::gl::glVertex3f -180.0 -50.0 50.0


#------------
pixel::gl::glVertex3f -180.0 -50.0 -50.0
pixel::gl::glVertex3f -180.0 -50.0 50.0

pixel::gl::glVertex3f -180.0 50.0 -50.0
pixel::gl::glVertex3f -180.0 50.0 50.0

pixel::gl::glVertex3f 180.0 -50.0 -50.0
pixel::gl::glVertex3f 180.0 -50.0 50.0

pixel::gl::glVertex3f 180.0 50.0 -50.0
pixel::gl::glVertex3f 180.0 50.0 50.0

pixel::gl::glEnd
box end
lappend objects box
# Draw box >>>

proc draw_screen {} {
	pixel::gl::glClear GL_COLOR_BUFFER_BIT GL_DEPTH_BUFFER_BIT
	pixel::gl::glMatrixMode GL_MODELVIEW
	pixel::gl::glLoadIdentity
	pixel::gl::glTranslatef 0.0 0.0 -$::viewdist
	pixel::gl::glRotatef $::angle 0.0 1.0 0.0
	pixel::gl::glRotatef $::angle2 0.0 0.0 1.0
	foreach obj $::objects {
		$obj draw
	}

	#pixel::gl::glEnd
	pixel::sdl::gl_swapbuffers
}


proc key {evname sdl_event} { #<<<
	dict with sdl_event {}

	if {$state eq "SDL_PRESSED"} {
		if {$ascii eq "q" || $sym eq 27} {
			exit
		}
	}
}

#>>>
proc expose {evname sdl_event} { #<<<
	pixel::sdl::do_frame $::scr
}

#>>>
proc quit {evname sdl_event} { #<<<
	exit 0
}

#>>>


pixel::sdl::bind_events key key
pixel::sdl::bind_events expose expose
pixel::sdl::bind_events quit quit

set ::angle		0.0
set ::angle2	0.0
set ::last	[clock clicks -milliseconds]
proc poll {} { #<<<
	pixel::sdl::dispatch_events
	update
	set now		[clock clicks -milliseconds]
	set elapsed	[expr {$now - $::last}]
	set ::last	$now
	set ::angle		[expr {$::angle + 30.0 * double($elapsed)/1000}]
	set ::angle2	[expr {$::angle2 + 20.0 * double($elapsed)/1000}]
	while {$::angle > 360} {
		set ::angle		[expr {$::angle - 360}]
	}
	while {$::angle2 > 360} {
		set ::angle2	[expr {$::angle2 - 360}]
	}

	draw_screen
	after 10 poll
}

#>>>

coroutine coro_main apply {
	{} {
		poll
		vwait ::forever
	}
}
