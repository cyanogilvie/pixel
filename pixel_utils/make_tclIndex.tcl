#!/usr/bin/tclsh

package require Itcl
namespace import -force itcl::*
auto_mkindex_parser::slavehook { _%@namespace import -force ::itcl::* }

cd scripts

auto_mkindex [pwd] *.tcl *.itk *.itcl
