project('Pixel_imlib2', 'c',
  version:          '4.0',
  default_options:  ['c_std=c17', 'warning_level=2'],
  meson_version:    '>=1.3.0',
)

cc = meson.get_compiler('c')
fs = import('fs')
add_project_arguments('-D_DEFAULT_SOURCE', language: 'c')

if get_option('debug')
  add_project_arguments(cc.get_supported_arguments(['-ggdb3']), language: 'c')
endif

add_project_arguments('-Wno-missing-field-initializers', language: 'c')

conf = configuration_data()
conf.set10('TESTMODE', get_option('testmode'))

deps = []
inc = []

subdir('teabase/init')

deps += dependency('pixel', version: '>=4.0')
deps += dependency('imlib2')
deps += dependency('freetype2')
deps += cc.find_library('m')

# imlib2's headers optionally need X11. Check if X11 is available and
# define X_DISPLAY_MISSING when not.
if not cc.has_header('X11/Xlib.h')
  add_project_arguments('-DX_DISPLAY_MISSING', language: 'c')
endif

pkg_sources = files(
  'generic/main.c',
)

# Staged alongside pkgIndex.tcl in both build and install dirs (via teabase).
teaconf_files = [files('library/utils.tcl')]

subdir('teabase')

# vim: foldmethod=marker foldmarker=<<<,>>> ts=2 shiftwidth=2 expandtab
