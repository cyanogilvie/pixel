#!/usr/bin/env cfkit8.6

lappend ::auto_path [file join / home cyan .tbuild repo pkg linux-glibc2.8-x86_64]
package require Pixel_gl
package require Pixel_sdl

try {
	set scr	[pixel::sdl::setup_screen 800 600 32 {SDL_ANYFORMAT SDL_OPENGL}]
	pixel::gl::glewInit
} trap {GLEW} {errmsg options} {
	lassign [dict get $options -errorcode] code glew_err
	puts stderr "Glew error ($glew_err): \"$errmsg\""
	exit 1
} on error {errmsg options} {
	puts stderr "Unexpected error in glewInit: $errmsg ([dict get $options -errorcode])"
	exit 1
} on ok {glew_version} {
	puts "Glew init ok, version: \"$glew_version\""
}

if {[pixel::gl::glewIsSupported "GL_VERSION_2_0"]} {
	puts "GL_VERSION_2_0 supported"
} else {
	puts "GL_VERSION_2_0 not supported"
}

if {[pixel::gl::glewIsSupported "GL_ARB_vertex_shader GL_ARB_fragment_shader"]} {
	puts "Shaders supported"
} else {
	puts "Shaders not supported"
}
