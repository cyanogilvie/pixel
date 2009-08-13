#!/usr/bin/env cfkit8.6

# vim: ft=tcl foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4

lappend auto_path	[file join ~ .tbuild repo pkg linux-glibc2.8-x86_64]

proc readfile {fn} { #<<<
	set fp	[open [file join [file dirname [info script]] $fn] r]
	try {
		chan read $fp
	} finally {
		catch {chan close $fp}
	}
}

#>>>

package require Pixel_sdl
package require Pixel_gl

set width	800
set height	600
set scr	[pixel::sdl::setup_screen $width $height 32 {SDL_ANYFORMAT SDL_OPENGL}]

namespace path [list {*}[namespace path] ::pixel::gl]

glewInit
if {![glewIsSupported "GL_VERSION_2_0"]} {
	puts stderr "GL version 2.0 not supported"
	exit 2
}

glMatrixMode GL_PROJECTION
glLoadIdentity
gluPerspective 45.0 [expr {$width / double($height)}] 0.1 100.0
glMatrixMode GL_MODELVIEW
glLoadIdentity
glClearColor 0.0 0.0 0.0 0.0
#glHint GL_PERSPECTIVE_CORRECTION_HINT GL_NICEST
#glHint GL_POINT_SMOOTH_HINT GL_NICEST
#glPointSize 1.0

proc shader_program {vert_fn frag_fn} { #<<<
	set p	[glCreateProgram]
	set v	[glCreateShader GL_VERTEX_SHADER]
	glShaderSource $v [readfile $vert_fn]
	try {
		glCompileShader $v
	} trap {GL COMPILE} {errmsg options} {
		puts stderr "Could not compile vertex shader \"$vert_fn\":\n$errmsg"
		exit 1
	}
	glAttachShader $p $v

	set f	[glCreateShader GL_FRAGMENT_SHADER]
	glShaderSource $f [readfile $frag_fn]
	try {
		glCompileShader $f
	} trap {GL COMPILE} {errmsg options} {
		puts stderr "Could not compile fragment shader \"$frag_fn:\n$errmsg"
		exit 1
	}
	glAttachShader $p $f

	glLinkProgram $p
	if {![glValidateProgram $p]} {
		puts stderr "Could not link shader program:\n[glGetProgramInfoLog $p]"
		exit 1
	}

	return [list $p $v $f]
}

#>>>

#lassign [shader_program "default.vert" "lambert.frag"] p v f
lassign [shader_program "wavey.vert" "wavey.frag"] p v f
puts "Program validation:\n[glGetProgramInfoLog $p]"
glUseProgram $p
set angleU	[glGetUniformLocation $p angle]

set plane	[glGenList]

set angle	0.0
set rotY	0.0
set npoints		50
set distance	0.1
glNewList $plane
glBegin GL_POINTS
for {set i 0} {$i < $npoints} {incr i} {
	for {set j 0} {$j < $npoints} {incr j} {
		glVertex3f \
				[expr {$npoints*(-$distance * 0.5) + $i * $distance}] \
				0.0 \
				[expr {$npoints*($distance * 0.5) - $j * $distance}]
	}
}
glEnd
glEndList

proc draw_screen {} { #<<<
	global angle last_frame_µsec rotY angleU plane npoints distance

	set nplanes	10

	if {![info exists last_frame_µsec]} {
		set last_frame_µsec	[clock microseconds]
	}

	set now		[clock microseconds]
	set delta_s	[expr {($now - ${last_frame_µsec}) / 1000000.0}]
	set last_frame_µsec	$now

	glClear GL_COLOR_BUFFER_BIT
	set angle	[expr {$angle + 300.0 * $delta_s}]
	while {$angle > 360} {
		set angle	[expr {$angle - 360}]
	}
	glUniform1f $angleU $angle
	set rotY	[expr {$rotY + 50.0 * $delta_s}]

	glLoadIdentity
	glTranslatef 0.0 2.75 -9.0
	glRotatef 30.0 1.0 0.0 0.0
	glRotatef $rotY 0.0 1.0 0.0

	for {set k 0} {$k < $nplanes} {incr k} {
		glTranslatef 0.0 [expr {-($npoints * $distance) / $nplanes}] 0.0
		glCallList $plane
	}

	pixel::sdl::gl_swapbuffers
}

#>>>

proc cleanup {} {
	glDeleteShader $::v
	glDeleteShader $::f
	glDeleteProgram $::p
	proc cleanup {} {}
}

pixel::sdl::bind_events key [list apply {
	{name ev} {
		array set e $ev
		puts stderr "key event:"
		parray e
		unset e
		dict with ev {}
		if {$state eq "SDL_PRESSED"} {
			if {$ascii eq "q" || $sym eq 27} {
				try {
					cleanup
				} on error {errmsg options} {
					puts stderr "Error cleaning up: [dict get $options -errorinfo]"
				}
				exit
			}
		}
	}
}]
pixel::sdl::bind_events expose [list apply {
	{name ev} {
		pixel::sdl::do_frame $::scr
	}
}]
pixel::sdl::bind_events quit [list apply {
	{name ev} {
		cleanup
		exit 0
	}
}]

proc poll {} {
	pixel::sdl::dispatch_events
	update
	draw_screen
	after 10 poll
}

coroutine coro_main apply {
	{} {
		poll
		vwait ::forever
	}
}

