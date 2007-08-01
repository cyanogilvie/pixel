#!/usr/bin/env tclsh

package require Itcl
namespace import -force itcl::*
auto_mkindex_parser::slavehook { _%@namespace import -force ::itcl::* }

cd generic/scripts

auto_mkindex [pwd] *.tcl *.itk *.itcl
