project('Pixel_webp', 'c',
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
# static:true so pkg-config --static picks up Libs.private (e.g. -lsharpyuv),
# which is needed because the alpine-tcl base image ships libwebp as .a only.
deps += dependency('libwebp', static: true)

pkg_sources = files(
  'generic/main.c',
)

subdir('teabase')

# vim: foldmethod=marker foldmarker=<<<,>>> ts=2 shiftwidth=2 expandtab
