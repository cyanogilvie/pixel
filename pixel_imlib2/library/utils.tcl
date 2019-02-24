namespace eval ::pixel::imlib2 {
	proc ::pixel::imlib2::decode bytes {
		set h	[file tempfile fn]
		try {
			chan configure $h -translation binary -encoding binary
			puts -nonewline $h $bytes
			flush $h
			pixel::imlib2::load_image $fn
		} finally {
			if {[info exists h] && $h in [chan names]} {close $h}
			file delete $fn
		}
	}

	proc ::pixel::imlib2::encode {format pmap} {
		set h	[file tempfile fn]
		try {
			chan configure $h -translation binary -encoding binary
			pixel::imlib2::save_image $pmap $fn $format
			seek $h 0
			read $h
		} finally {
			if {[info exists h] && $h in [chan names]} {close $h}
			file delete $fn
		}
	}
}