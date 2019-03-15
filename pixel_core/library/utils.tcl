namespace eval ::pixel {
	namespace export *
	namespace ensemble create -prefixes no

	proc _shear_rotate {p deg} { #<<<
		if {$deg == 0} {
			return $p
		}

		set pi		3.14159265
		set theta	[expr {$deg * $pi/180}]
		set a		[expr {-tan($theta/2.0)}]
		set b		[expr { sin($theta)}]

		lassign [pixel::pmapf_info $p] w h
		set p	[pixel::shear_pmapf_lanczos_x $p $a]
		set skip	[expr {int(abs($h * $a * $b))}]
		set p	[pixel::shear_pmapf_lanczos_y $p $b $skip $skip]
		lassign [pixel::pmapf_info $p] s2_w s2_h
		set skip	[expr {int(($s2_h - ($h - $skip)) * abs($a))}]

		pixel::shear_pmapf_lanczos_x $p $a $skip $skip
	}

	#>>>
	proc rotate {p deg} { # Rotates $deg clockwise <<<
		if {$deg >= 360} {
			set deg	[expr { fmod($deg, 360)}]
		} elseif {$deg <= 360} {
			set deg	[expr {-fmod($deg*-1, 360)}]
		}
		# deg (-360, 360)

		if {$deg > 180}   {set deg [expr {$deg - 360}]}
		if {$deg <= -180} {set deg [expr {$deg + 360}]}
		# deg (-180, 180]

		if {abs($deg) <= 45} {
			_shear_rotate $p $deg
		} elseif {$deg > 45 && $deg < 135} {
			_shear_rotate [pixel::pmapf_rotate_90 $p  1] [expr {$deg-90}]
		} elseif {$deg > -135 && $deg < -45} {
			_shear_rotate [pixel::pmapf_rotate_90 $p -1] [expr {$deg+90}]
		} else {
			_shear_rotate [pixel::pmapf_rotate_90 $p  2] [expr {$deg<0 ? $deg+180 : $deg-180}]
		}
	}

	#>>>
}

# vim: ft=tcl foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
