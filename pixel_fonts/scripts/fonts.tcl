# vim: ft=tcl foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4

oo::class create pixel::fonts::Font {
	variable {*}{
		reg
		attribmap
	}

	constructor {} { #<<<
		package require cflib

		array set reg	{}
		set attribmap(normal)	0
		set exp					0
		foreach attrib {
			normal bold italic mono
		} {
			set attribmap($attrib)	[expr {1 << $exp}]
			incr exp
		}

		my add_font_spec [file join $::pixel::fonts::library fonts fonts.spec]
	}

	#>>>

	method add_font_spec {specfile} { #<<<
		if {![file exists $specfile]} {
			error "Specfile doesn't exist: ($specfile)"
		}

		set dat	[cflib::readfile $specfile]

		foreach {basename spec} $dat {
			catch {unset tmp}
			array set tmp {
				attribs			{}
			}
			array set tmp $spec
			if {![info exists tmp(filename)]} {
				error "font $basename in $specfile doesn't give valid filename"
			}
			set fn	[file join [file dirname $specfile] $tmp(filename)]
			if {![file exists $fn]} {
				error "font $basename in $specfile doesn't give valid filename"
			}
			set attribs		$tmp(attribs)
			set filename	$fn

			my add_font $basename $attribs $filename
		}
	}

	#>>>
	method add_font {basename attribs filename} { #<<<
		#puts "add_font: ($basename) ($attribs) ($filename)"
		set name		[my _compile_fontname $basename $attribs]
		set hold		[pwd]
		cd [file dirname $basename]
		set reg($name)	[list [file join [pwd] $filename]]
		cd $hold
		#puts "\t$name - $reg($name)"
	}

	#>>>
	method get_face {fontdesc} { #<<<
		lassign $fontdesc basename attribs
		set name	[my _compile_fontname $basename $attribs]
		if {![info exists reg($name)]} {
			parray reg
			error "No such font: $basename, $attribs"
		}

		# Force face compilation on our copy, so it's only done once per font
		pixel::ttf::compile_face $reg($name)

		return $reg($name)
	}

	#>>>
	method list_basenames {} { #<<<
		set build	{}
		foreach name [array names reg] {
			#puts "list_basenames: processing ($name)"
			lappend build	[lindex [split $name ,] 0]
		}

		lsort -dictionary -unique $build
	}

	#>>>
	method list_attribs {basename} { #<<<
		set build	{}
		foreach name [array names reg "$basename,*"] {
			set tmp		[lindex [split $name ,] 1]
			set thisrow	{}
			foreach {attrib aid} [array get attribmap] {
				if {$tmp & $aid} {
					lappend thisrow	$attrib
				}
			}
			lappend build	$thisrow
		}
		return $build
	}

	#>>>
	method add_ttf_files {files} { #<<<
		foreach file $files {
			if {![file exists $file]} {
				error "File ($file) doesn't exist"
			}
			array set altinf	{}
			try {
				pixel::ttf_info [list $file]
			} on error {errmsg options} {
				error "Couldn't get font info from ($file): $errmsg"
			} on ok {inf} {}

			foreach item $inf {
				array unset fields
				array set fields $item
				if {
					$fields(platform) eq "TT_PLATFORM_MACINTOSH" &&
					[string trim $fields(string)] ne ""
				} {
					set altinf($fields(type))	$fields(string)
					if {$fields(type) eq "TT_NAME_ID_FONT_SUBFAMILY"} {
						if {![info exists used($fields(string))]} {
							set used($fields(string))	1
						} else {
							incr used($fields(string))
						}
					}
				}
			}

			if {![info exists altinf(TT_NAME_ID_FONT_SUBFAMILY)]} {
				error "Can't get subfamily info from ($file)"
			}
			if {![info exists altinf(TT_NAME_ID_FONT_FAMILY)]} {
				error "Can't get family info from ($file)"
			}
			set raw		[string tolower $altinf(TT_NAME_ID_FONT_SUBFAMILY)]
			set attribs	{}
			foreach attrib $raw {
				switch -- $attrib {
					"regular" -
					"normal" {
						continue
					}

					"oblique" {
						lappend attribs	"italic"
					}

					default {
						if {$attrib ni {normal bold italic mono}} continue
						lappend attribs	$attrib
					}
				}
			}
			#puts "$file - {[list [string tolower $altinf(TT_NAME_ID_FONT_FAMILY)] $attribs]}"
			my add_font $altinf(TT_NAME_ID_FONT_FAMILY) $attribs $file
		}
	}

	#>>>
	method _compile_fontname {basename attribs} { #<<<
		set name	[string tolower [string trim $basename]]
		append name	,
		set aid		0
		foreach attrib $attribs {
			set attrib	[string tolower [string trim $attrib]]
			if {[info exists attribmap($attrib)]} {
				incr aid	$attribmap($attrib)
			} else {
				error "No such font attribute: ($attrib)"
			}
		}
		append name	$aid

		return $name
	}

	#>>>
}


