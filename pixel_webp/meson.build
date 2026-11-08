project('Pixel_webp', 'c',
  version:          '4.1',
  default_options:  ['c_std=c2y,c23,c2x', 'warning_level=2'],
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

# parseargs_bind.def (pixel::webp::encode's options) releases what it binds with defer
tea_want_defer = true

subdir('teabase/init')

deps += dependency('pixel', version: '>=4.0')
deps += dependency('parse_args', version: '>=0.7.2')
# static:true so pkg-config --static picks up Libs.private (e.g. -lsharpyuv),
# which is needed because the alpine-tcl base image ships libwebp as .a only.
libwebp_dep = dependency('libwebp', static: true)
deps += libwebp_dep
teaconf.set('LIBWEBP_VERSION', libwebp_dep.version())  # For the manpage

pkg_sources = files(
  'generic/main.c',
)

subdir('teabase')
subdir('doc')

# vim: foldmethod=marker foldmarker=<<<,>>> ts=2 shiftwidth=2 expandtab
