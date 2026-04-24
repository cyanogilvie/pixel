project('Pixel_phash', ['c', 'cpp'],
  version:          '4.0',
  default_options:  ['c_std=c17', 'cpp_std=c++14', 'warning_level=2'],
  meson_version:    '>=1.3.0',
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
fs = import('fs')
add_project_arguments('-D_DEFAULT_SOURCE', language: ['c', 'cpp'])

if get_option('debug')
  add_project_arguments(cc.get_supported_arguments(['-ggdb3']), language: 'c')
  add_project_arguments(cxx.get_supported_arguments(['-ggdb3']), language: 'cpp')
endif

add_project_arguments('-Wno-missing-field-initializers', language: ['c', 'cpp'])

# teabase/init only adds HAVE_CONFIG_H for C; pixel_phash's main.cpp needs it too.
add_project_arguments('-DHAVE_CONFIG_H=1', language: 'cpp')

conf = configuration_data()
conf.set10('TESTMODE', get_option('testmode'))

deps = []
inc = []

subdir('teabase/init')

# Pixel_core stubs — pixel.h is included from C++ via extern "C"; the
# -DUSE_PIXEL_STUBS flag must be applied to cpp as well (teabase already
# does this for USE_TCL_STUBS).
add_project_arguments('-DUSE_PIXEL_STUBS=1', language: 'cpp')
deps += dependency('pixel', version: '>=4.0')

# pHash — nearly undocumented upstream. The subproject wrap provides a
# meson build for the pinned dea9ffc revision. default_library=static so
# libtcl9Pixel_phash.so is self-contained on the pHash side.
deps += dependency('phash',
  fallback: ['phash', 'phash_dep'],
  default_options: ['default_library=static'])

deps += dependency('libtiff-4', static: true)
deps += dependency('libpng',    static: true)
# CImg.h uses libwebp conditionally; pHash.a links against its symbols even
# when we don't touch image I/O from our own code. Add it explicitly so the
# static link is closed.
deps += dependency('libwebp',   static: true)

pkg_sources = files(
  'generic/main.cpp',
  'generic/digest_type.cpp',
)

subdir('teabase')

# vim: foldmethod=marker foldmarker=<<<,>>> ts=2 shiftwidth=2 expandtab
