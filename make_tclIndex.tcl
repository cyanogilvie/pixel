#!/usr/bin/itclsh3.1

package require Itcl
namespace import -force itcl::*

cd scripts

auto_mkindex [pwd] *.tcl *.itk *.itcl
